#include "sorry.h"
#include "math_utils.h"

/* #define DISABLE_MOTORS */

#define ULTRA_ULTRA_POWER 180
#define ULTRA_POWER 140
#define FAST_POWER 83
#define MEDIUM_POWER 76
#define SLOW_POWER 62
#define STOPPED_POWER 0

#define MAX_OUTPUT_POWER 110

#define TRACK_DIM 6  // ft
#define L_Y_DELTA 0.3958  // ft
#define L_X_OFFSET 0.213  // ft

#define MAX_TURN_IN_PLACE_OUTPUT_POWER 140//75 // must be < 255
#define MAX_TURN_IN_PLACE_ERROR_I 1200
#define MAX_DRIFT_ERROR_I 200

#define LOWER_RIGHT_VEL_SP_BY 0.94

#define DRIFT_LOOK_AHEAD_DIST 1.5

#define STOP_OFFSET 0.3
#define REVERSE_SPEED -40
#define REVERSE_TIME 1000

#define MAX_TURN_IN_PLACE_ERROR_I 700

#define kP_drift 0.55
#define kD_drift 900
#define kI_drift 0.002

#define TURN_IN_PLACE_TIMEOUT 2000000

// ready to start calibration once the gyro has calmed down after init
// (at init, it drifts like 10 deg / sec which is way faster than we actually expect and will be calibrating for.)
#define MAX_DEGREES_PER_SECOND_BEFORE_CALIBRATION 0.2
// after 8 seconds, give up on gyro calming down more and just calibrate that mofo
#define MAX_WAIT_FOR_DUMB_GYRO_SECONDS 10

#define GYRO_CALIBRATION_DURATION_MS 3000
#define NUM_GYRO_TICKS_WITHIN_THRESHOLD_AFTER_WHICH_TO_START_CALIBRATION 15

#define MAXIMUM_WALL_ANGLE_DIVERGENCE_BEFORE_DISTRUSTING_LEFT_TOFS 16 // if gyro angle and wall angle diverge by more than this during a single drive forward segment, just trust gyro angle!

#define MAX_TOLERABLE_WALL_DISTANCE_DIVERGENCE 1


// MAJOR TODOs
// 0) Don't bother updating back TOF 99% of the time, we only need it for possibly TODO #3 (add a flag to sensors::update()

// 1) Calibrate gyro at start,
//   1b) keep a running curAngle, account for gyro drift every tick.
//   1c) when running getWallAngle, if the angle calculated differs sufficiently from the gyro's measurement, just trust the gyro. In that case, just re-use the previous wall distance (distToLeftWallError) for correction.

// 2) If front or left TOF readings are bad for long enough, just stop the robot entirely I guess
//
// 3) Use either backTof distance or the correct gyro axis to calculate when we've overcome the trap lip, make some kind of new threshhold argument to drive() with a default value <- eg. by default dont use this new threshold so we can enable it for just the trap segment
//
// 4) if curDistToLeftWall differs by the expected by more than 1 tile, use GUIDED_GYRO correction instead of GUIDED
// 4b) if this persists for more than some threshold like 1 second or something, stop robot entirely
//
// 5) do all TOF i2c commands from seperate thread, see https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
// 5b) if thread hangs kill it ASAP and try again

Sorry::Sorry(Motors* motors, Sensors* sensors, Nav* nav, Hms* hms):
motors(motors),
sensors(sensors),
hms(hms),
nav(nav){
  errDriftI = 0;
  angleError = 0;
  angFromWall = 0;
  deltaT = 0;
  curT = micros();
  gyroDriftPerMicro = 0;
  numGyroClockwiseWraparounds = 0; // in order to keep our units consistent for angle comparisons
  prevGyroMeasurementWasLargeAndMightWrapAroundToNegative = false;
  startCurDriveSegmentT = 0;
  startCurDriveSegmentAngle = 0;
}

float Sorry::bringGyroMeasurementIntoPositiveDegreesUsingNumClockwiseWraparounds(float rawAngle){
  float correctedAngle = rawAngle;
  if (rawAngle < 0 && prevGyroMeasurementWasLargeAndMightWrapAroundToNegative){
    correctedAngle += 360;
    numGyroClockwiseWraparounds++;
  }
  correctedAngle += 360 * numGyroClockwiseWraparounds;
  prevGyroMeasurementWasLargeAndMightWrapAroundToNegative = rawAngle > 150;
  /* Serial.print("bring to positive -> rawAngle: "); Serial.print(rawAngle); Serial.print(" +360*n: "); Serial.println(correctedAngle); */
  return correctedAngle;
}

void Sorry::calibrateGyroDrift(){
  unsigned long preCalibrationT;

  // start by waiting for gyro to calm the fuck down because when it first inits it sucks dick
  float prevAngle = getDirectionCorrectedGyroAngle();
  unsigned long lastTimestamp = micros();
  delay(5);
  int numSatisfactoryTicks = 0; // number of ticks where gyro drift is slow enough
  while(micros()-preCalibrationT<MAX_WAIT_FOR_DUMB_GYRO_SECONDS*1000000){
    delay(5);
    float curAngle = getDirectionCorrectedGyroAngle();
    if (curAngle == prevAngle){
      while(true){
        Serial.println("Gyro read the same value twice, probably broken because it should be drifting rn!!!!!");
        delay(1000);
      }
    }
    unsigned long curTimestamp = micros();
    unsigned long deltaMicros = curTimestamp - lastTimestamp;
    if ((abs(curAngle - prevAngle)/deltaMicros)<MAX_DEGREES_PER_SECOND_BEFORE_CALIBRATION/1000000){
      numSatisfactoryTicks++;
    }
    else{
      numSatisfactoryTicks = 0;
    }
    if (numSatisfactoryTicks > NUM_GYRO_TICKS_WITHIN_THRESHOLD_AFTER_WHICH_TO_START_CALIBRATION){
      Serial.println("achieved threshold");
      break; // gyro has been drifting slow enough for long enough, so we are ready to calibrate
    }
    lastTimestamp = curTimestamp;
    prevAngle = curAngle;
  }
  Serial.println("done pre calibration");
  float firstAngle = getDirectionCorrectedGyroAngle();
  unsigned long startCalibrationT = micros();
  delay(5);
  while(micros()-startCalibrationT<GYRO_CALIBRATION_DURATION_MS*1000){
    getDirectionCorrectedGyroAngle();
    delay(5);
  }
  float lastAngle = getDirectionCorrectedGyroAngle();
  unsigned long endCalibrationT = micros();

  gyroDriftPerMicro = (lastAngle - firstAngle)/(endCalibrationT - startCalibrationT);
  Serial.printf("firstAngle: %6.3f lastAngle: %6.3f", firstAngle, lastAngle);
  Serial.printf("gyroDriftPerMicro: %15.14f\n", gyroDriftPerMicro);
  Serial.printf("gyroDriftPerSecond: %15.12f\n", gyroDriftPerMicro*1000000);
}

void Sorry::run(){
  calibrateGyroDrift();
  /* drive(FAST_POWER,    800,  0.5,  GUIDED        ); */
  /* drive(MEDIUM_POWER,  200,  0.5,  GUIDED        ); */
  /* drive(SLOW_POWER,    2000, 0.5,  GUIDED,   0.55); */
  /* drive(STOPPED_POWER, 500,  0.5,  GUIDED        ); */
  /* turnInPlace(); */
  /* drive(FAST_POWER,    800,  0.5,  GUIDED        ); */
  /* drive(MEDIUM_POWER,  200,  0.5,  GUIDED        ); */
  /* drive(SLOW_POWER,    2000, 0.5,  GUIDED,   0.55); */
  /* drive(STOPPED_POWER, 500,  0.5,  GUIDED        ); */
  /* turnInPlace(); */
  /* drive(FAST_POWER,    800,  0.5,  GUIDED        ); */
  /* drive(MEDIUM_POWER,  200,  0.5,  GUIDED        ); */
  /* drive(SLOW_POWER,    2000, 0.5,  GUIDED,   0.48); */
  /* drive(STOPPED_POWER, 500,  0.5,  PARALLEL      ); */
  /* turnInPlace(); */
  // segment 3: deep pit to climb
  drive(FAST_POWER,  3100, 0.4,  GUIDED_GYRO   );
  /* drive(ULTRA_ULTRA_POWER,260,0.4, UNGUIDED      ); */
  drive(SLOW_POWER,    2000, 0.4,  GUIDED,   1.50);
  drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  turnInPlace();
  drive(FAST_POWER,    1000, 1.47, GUIDED        );
  drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  drive(STOPPED_POWER, 500,  0.4,  GUIDED        );
  turnInPlace();
  drive(FAST_POWER,    1000, 1.47, GUIDED        );
  drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  turnInPlace();
  drive(FAST_POWER,    1000, 1.47, GUIDED        );
  drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  turnInPlace();
  drive(FAST_POWER,    1000, 1.47, GUIDED        );
  drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  turnInPlace();
}

void Sorry::drive(float motorPower, unsigned long timeout, float desiredDistToLeftWall, CorrectionMode correctionMode, float distanceToStopAt){
  timeout *= 1000;
  startCurDriveSegmentT = micros();
  curT = micros();
  firstTick = true;
  while(true){
    deltaT = micros() - curT;
    curT += deltaT;
    float frontDist = getTofFt(0);
    if (distanceToStopAt > 0){
      Serial.printf("frontD: %5.4f distToStop: %5.4f\n", frontDist, distanceToStopAt);
      if (frontDist <= distanceToStopAt + STOP_OFFSET){
        break;
      }
    }
    if (curT - startCurDriveSegmentT >= timeout){
      break;
    }
    /* nav->getGyroAngle(); // literally just so fusion updates */
    driveTick(motorPower, desiredDistToLeftWall, correctionMode, firstTick);
    firstTick = false;
  }

  if (motorPower == STOPPED_POWER){
    motors->setAllToZero();
  }
}

float Sorry::getDirectionCorrectedGyroAngle(){
  return bringGyroMeasurementIntoPositiveDegreesUsingNumClockwiseWraparounds(nav->getGyroAngle());
}

// correct a gyro angle 
float Sorry::getDriftCorrectedGyroAngle(unsigned long microsSinceLastZeroed){
  float directionCorrectedAngle = getDirectionCorrectedGyroAngle();
  float driftCorrectedAngle = directionCorrectedAngle - microsSinceLastZeroed * gyroDriftPerMicro;
  return driftCorrectedAngle;
}

bool Sorry::isValid(int tofNum){
  return !(getTofFt(tofNum) > TRACK_DIM  || getTofFt(tofNum) <= 0);
}

float Sorry::getTofFt(int tofNum){
  return sensors->tof[tofNum].getData().dist * 0.00328084; //mm -> ft
}

bool Sorry::updateWallAngleAndDistance(float gyroAngle, float desiredDistToLeftWall, bool firstTick){
  if (!(isValid(1) && isValid(2))){
    return false;
  }
  angFromWall = rad2deg(atan((getTofFt(1) - getTofFt(2)) / L_Y_DELTA));

  curDistToLeftWall = ((getTofFt(1) + getTofFt(2)) / 2 + L_X_OFFSET) * cosd(angFromWall);
  if (abs(curDistToLeftWall - desiredDistToLeftWall) > MAX_TOLERABLE_WALL_DISTANCE_DIVERGENCE){
    return false;
  }

  if (firstTick){
    return true;
  }
  return abs(abs(gyroAngle - angFromWall) - abs(gyroAngleAtLastValidWallAngle - lastValidWallAngle)) < MAXIMUM_WALL_ANGLE_DIVERGENCE_BEFORE_DISTRUSTING_LEFT_TOFS;
}

void Sorry::driveTick(float motorPower, float desiredDistToLeftWall, CorrectionMode correctionMode, bool firstTick){
  float gyroAngle = 0;
  float desiredAngle = 0;
  if (correctionMode == UNGUIDED){
    nav->getGyroAngle();
#ifndef DISABLE_MOTORS
    motors->setPower(motorPower, motorPower*LOWER_RIGHT_VEL_SP_BY);
#endif
    return;
  }
  sensors->update(correctionMode==GUIDED_GYRO); // the bool param is skipTOFs which we do only in guided_gyro mode
  if (firstTick){
    startCurDriveSegmentAngle = getDirectionCorrectedGyroAngle();
    gyroAngle = startCurDriveSegmentAngle;
  }
  else{
    gyroAngle = getDriftCorrectedGyroAngle(curT - startCurDriveSegmentT);
  }
  float curAngle;
  if (correctionMode == GUIDED_GYRO){
      desiredAngle = startCurDriveSegmentAngle;
      curAngle = gyroAngle;
  }
  else if (updateWallAngleAndDistance(gyroAngle, desiredDistToLeftWall, firstTick) && correctionMode == GUIDED){
    float distToLeftWallError = curDistToLeftWall - desiredDistToLeftWall;
    desiredAngle = -rad2deg(atan(distToLeftWallError/DRIFT_LOOK_AHEAD_DIST));
    lastValidWallAngle = angFromWall;
    gyroAngleAtLastValidWallAngle = gyroAngle;
    curAngle = angFromWall;
    if (correctionMode == PARALLEL){
      desiredAngle = 0;
    }
  }
  else{
    desiredAngle = startCurDriveSegmentAngle - (gyroAngleAtLastValidWallAngle - lastValidWallAngle);
    curAngle = gyroAngle;
  }

  // DRIFT PID --------------------------------------------------------------------------
  float lastAngleError = angleError;
  angleError = curAngle - desiredAngle;

  float errDriftD = (angleError - lastAngleError)*1000/deltaT;
  errDriftI += angleError * deltaT/1000;
  if (sign(lastAngleError) != sign(angleError)){
    errDriftI = 0;
  }
  if (firstTick){
    errDriftI = 0;
    errDriftD = 0;
  }
  errDriftI = constrainVal(errDriftI, MAX_DRIFT_ERROR_I);

  float P = angleError * kP_drift;
  float I = errDriftI * kI_drift;
  float D = errDriftD * kD_drift;

  float driftOutput = P + I + D;

  driftOutput *= motorPower / MAX_OUTPUT_POWER;
  if (motorPower == SLOW_POWER){ // TODO scale motorPower!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    driftOutput *= 0.6;
  }

  float rightOutputDrift = driftOutput;
  float leftOutputDrift = -driftOutput;
    
  float leftTotal = motorPower + leftOutputDrift;
  float rightTotal = motorPower + rightOutputDrift;

  if (rightTotal > MAX_OUTPUT_POWER){
    float spillover = rightTotal/MAX_OUTPUT_POWER;
    rightTotal /= spillover;
    leftTotal /= spillover;
  }
  else if (rightTotal < -MAX_OUTPUT_POWER){
    float spillover = -rightTotal/MAX_OUTPUT_POWER;
    rightTotal /= spillover;
    leftTotal /= spillover;
  }
  else if (leftTotal > MAX_OUTPUT_POWER){
    float spillover = leftTotal/MAX_OUTPUT_POWER;
    rightTotal /= spillover;
    leftTotal /= spillover;
  }
  else if (leftTotal < -MAX_OUTPUT_POWER){
    float spillover = -leftTotal/MAX_OUTPUT_POWER;
    rightTotal /= spillover;
    leftTotal /= spillover;
  }
  leftTotal = constrainVal(leftTotal, MAX_OUTPUT_POWER);
  rightTotal = constrainVal(rightTotal, MAX_OUTPUT_POWER);
  Serial.printf("curAngle=%7.3f wallAngle %7.3f gyroAngle %7.3f desiredAngle=%7.3f lDr=%4.1f rDr=%4.1f dt=%7.3f bat=%7.3f\n",
        curAngle, angFromWall, gyroAngle, desiredAngle, leftOutputDrift, rightOutputDrift,
        hms->data.batteryVoltage);
  motors->setPower(leftTotal, rightTotal*LOWER_RIGHT_VEL_SP_BY);
}

void Sorry::turnInPlace(){
  float threshold = 3; // end loop when 3 degrees from donezo for thresholdTime sec
  unsigned long thresholdTime = 50000;
  float angleDelta = 0;
  float DUMB_ERROR_OFFSET = 0;
  float turnAmount = 90 + DUMB_ERROR_OFFSET;
  float error = turnAmount;
  float lastError = error;
  float kp_turny = 1.5;
  float kd_turny = 400;
  float ki_turny = 0.18;
  unsigned long _firstT = micros();
  unsigned long lastTimestamp = micros(); // zach I pinky promise that these two timestamps
  // will not be subtracted from each other and result in divide by zero errors.
  unsigned long successTime = 0;

  float curTs;
  float enterThresholdTimestamp;
  bool withinThreshold = false;
  float _deltaT;
  float errorD;
  float P;
  float errorI;
  float I;
  float D;
  float total;
  float rawAngle;

  float maxPower = MAX_TURN_IN_PLACE_OUTPUT_POWER * (5-(hms->data.batteryVoltage)/4);

  // theres a timeout dont worry
  Serial.println("Start turny");
  float startAngle = getDirectionCorrectedGyroAngle();
  float curAngle = startAngle;

  while(true){
    curTs = micros();
    curAngle = getDriftCorrectedGyroAngle(curTs - _firstT); //nav->getGyroAngle();
    /* if (curAngle - rawAngle > 300){ //300 since cur - new will loop over to 360 degrees, but not quite 360 */
      /* curAngle = 360 + rawAngle; */
    /* } */
    /* else{ */
      /* curAngle = rawAngle; */
    /* } */
    angleDelta = curAngle - startAngle;
    error = turnAmount - angleDelta;
    if (fabs(error) < threshold){
      if (withinThreshold){
        if (curTs - enterThresholdTimestamp > thresholdTime){
          Serial.println("Donezo");
          break;
        }
      }
      else{
        withinThreshold = true;
        enterThresholdTimestamp = curTs;
      }
    }
    else{
      withinThreshold = false;
    }
    
    _deltaT = curTs - lastTimestamp;
    lastTimestamp = curTs;

    /* if it takes longer than 4 seconds to turn, you fucked up */
    if (curTs - _firstT > TURN_IN_PLACE_TIMEOUT){
      Serial.println("turn better next time please");
      break;
    }
    errorD = (error - lastError)*1000/_deltaT;
    errorI += error * _deltaT/1000;
    if (sign(lastError) != sign(error)){
      errorI = 0;
    }
    errorI = constrainVal(errorI, MAX_TURN_IN_PLACE_ERROR_I);
    P = error * kp_turny;
    I = errorI * ki_turny;
    D = errorD * kd_turny;

    total = P + I + D;
    /* total = I;// + D; */

    total = constrainVal(P + I + D, maxPower);
    Serial.printf("StartAngle: %.3f curAngle(adj): %.3f | P: %.3f * %.3f = %.3f D: %.3f * %.3f = %.3f | I: %.3f * %.3f = %.3f | L: %.3f, R: %.3f\n", startAngle, curAngle, error,kp_turny,P, errorD,kd_turny,D, errorI, ki_turny, I, total, -total);
#ifndef DISABLE_MOTORS
    motors->setPower(total, -total*LOWER_RIGHT_VEL_SP_BY);
#endif
    lastError = error;

  }
  motors->setAllToZero();
  return;
}
