#include "sorry.h"
#include "math_utils.h"

#define ULTRA_POWER_MWAHAHAHA 110
#define MOUNT_WALL_POWER 82
#define MOUNT_WALL_POWER2 110
#define FAST_POWER 85
#define MEDIUM_POWER 70
#define SLOW_POWER 62
#define STOPPED_POWER 0
#define BRAKING_POWER -100
/* #define MEDIUM_POWER 55 */
/* #define SLOW_POWER 44 */

#define MAX_OUTPUT_POWER 120

#define TRACK_DIM 6  // ft
#define L_Y_DELTA 0.3958  // ft
#define L_X_OFFSET 0.213  // ft

#define MAX_TURN_IN_PLACE_OUTPUT_POWER 60//75 // must be < 255
#define MAX_TURN_IN_PLACE_ERROR_I 1200
#define MAX_DRIFT_ERROR_I 200

#define LOWER_RIGHT_VEL_SP_BY 0.87
#define DRIFT_LOOK_AHEAD_DIST 1.5

#define STOP_OFFSET 0.6
#define REVERSE_SPEED -40
#define REVERSE_TIME 1000

#define MAX_TURN_IN_PLACE_ERROR_I 700

/* #define kP_drift 0.55 */
/* #define kD_drift 900 */
/* #define kI_drift 0.002 */
#define kP_drift 1.2
#define kD_drift 900
#define kI_drift 0.0

#define TURN_IN_PLACE_TIMEOUT 2000000
// ready to start calibration once the gyro has calmed down after init
// (at init, it drifts like 10 deg / sec which is way faster than we actually expect and will be calibrating for.)
#define MAX_DEGREES_PER_SECOND_BEFORE_CALIBRATION 0.2
// after 8 seconds, give up on gyro calming down more and just calibrate that mofo
#define MAX_WAIT_FOR_DUMB_GYRO_SECONDS 6

#define GYRO_CALIBRATION_DURATION_MS 3000
#define NUM_GYRO_TICKS_WITHIN_THRESHOLD_AFTER_WHICH_TO_START_CALIBRATION 15

#define MAX_TOLERABLE_WALL_DISTANCE_DIVERGENCE 1

Sorry::Sorry(Motors* motors, Sensors* sensors, Hms* hms):
motors(motors),
sensors(sensors),
hms(hms){
  angleErrorI = 0;
  angleError = 0;
  angFromWall = 0;
  deltaT = 0;
  gyroDriftPerMicro = 0;
  numGyroClockwiseWraparounds = 0; // in order to keep our units consistent for angle comparisons
  prevGyroMeasurementWasLargeAndMightWrapAroundToNegative = false;
  curT = micros();
}

void Sorry::run(){
  calibrateGyroDrift();

  // FIRST LINE (START) -------------------------------------------
  /* return; */
  drive(0,0,FAST_POWER,    800,  0.5,  GUIDED        );
  drive(0,1,MEDIUM_POWER,  200,  0.5,  GUIDED        );
  drive(0,2,SLOW_POWER,    2000, 0.5,  GUIDED,   0.55);
  /* drive(0,3,STOPPED_POWER, 500,  0.5,  GUIDED        ); */
  turnInPlace(0);

  // SECOND LINE -------------------------------------------
  /* drive(0, 800000, 0, 200000, 0.55, 0.5); */
  drive(1,0,FAST_POWER,    800,  0.5,  GUIDED        );
  drive(1,1,MEDIUM_POWER,  200,  0.5,  GUIDED        );
  drive(1,2,SLOW_POWER,    2000, 0.5,  GUIDED,   0.55);
  /* drive(1,3,STOPPED_POWER, 500,  0.5,  GUIDED        ); */
  turnInPlace(1);

  // THIRD LINE
  /* drive(0, 800000, 0, 200000, 0.53, 0.5); */
  drive(2,0,FAST_POWER,    800,  0.5,  GUIDED        );
  drive(2,1,MEDIUM_POWER,  200,  0.5,  GUIDED        );
  drive(2,2,SLOW_POWER,    2000, 0.5,  PARALLEL,   0.46);
  /* drive(2,3,STOPPED_POWER, 500,  0.5,  PARALLEL      ); */
  turnInPlace(2);
  drive(3,0,MOUNT_WALL_POWER,  3200, 0.4,  GUIDED, -1, -160, false);
  drive(3,1,MOUNT_WALL_POWER2,  400, 0.4,  UNGUIDED, -1, 180, true);
  drive(3,2,SLOW_POWER, 600, 0.5,  PARALLEL, 2.15);

  // YOU ARE NOW OUT OF PIT
  turnInPlace(3);
  drive(4,0,FAST_POWER,    1000, 1.47, GUIDED        );
  drive(4,1,MEDIUM_POWER,  300, 1.47, GUIDED        ); // we are tuning this rn, it was too long @ 800
  drive(4,2,SLOW_POWER,    2000, 1.47, PARALLEL,   1.50);

  turnInPlace(4);
  drive(5,0,FAST_POWER,    1000, 1.47, GUIDED        );
  drive(5,1,MEDIUM_POWER,  200, 1.47, GUIDED        );
  drive(5,2,SLOW_POWER,    2000, 1.47, PARALLEL,   1.50);

  turnInPlace(5);
  drive(6,0,FAST_POWER,    800, 1.47, GUIDED        );
  drive(6,1,MEDIUM_POWER,  200, 1.47, GUIDED        );
  drive(6,2,SLOW_POWER,    2000, 1.47, PARALLEL,   1.50);
  turnInPlace(6);
  /* drive(0, 1000000, 0, 1200000, 2.5, 1.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 2.5, 2.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 2.5, 2.47); */
  drive(7,0,FAST_POWER,    500, 1.47, GUIDED        );
  drive(7,2,SLOW_POWER,    2000, 1.47, PARALLEL,   2.60);
  turnInPlace(7);
  drive(8,0,FAST_POWER,    500, 2.47, GUIDED        );
  drive(8,2,SLOW_POWER,    2000, 2.47, PARALLEL,   2.60);
  turnInPlace(8);
  // drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );

  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 1.35, 1.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 1.5, 1.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 1.2, 1.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 1.5, 1.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 2.5, 1.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 2.5, 2.47); */
  /* turnInPlace(); */
  /* drive(0, 1000000, 0, 1200000, 2.5, 2.47); */
  /* turnInPlace(); */
  /* drive(0, 500000, 0, 500000, 2.5, 2.47); */
  // // segment 3: deep pit to climb
  // // drive(FAST_POWER,  3100, 0.4,  GUIDED_GYRO     );
  // drive(MEDIUM_POWER,  3300, 0.4,  GUIDED, -1, 20); //The -1 is the flag for not reading distance to stop at (otherwise defaulted argument)
  // drive(SLOW_POWER,    2000, 0.4,  GUIDED,   1.50);
  // drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  // turnInPlace();
  // drive(FAST_POWER,    1000, 1.47, GUIDED        );
  // drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  // drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  // drive(STOPPED_POWER, 500,  0.4,  GUIDED        );
  // turnInPlace();
  // drive(FAST_POWER,    1000, 1.47, GUIDED        );
  // drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  // drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  // drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  // turnInPlace();
  // drive(FAST_POWER,    1000, 1.47, GUIDED        );
  // drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  // drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  // drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  // turnInPlace();
  // drive(FAST_POWER,    1000, 1.47, GUIDED        );
  // drive(MEDIUM_POWER,  1200, 1.47, GUIDED        );
  // drive(SLOW_POWER,    2000, 1.47, GUIDED,   1.50);
  // drive(STOPPED_POWER, 500,  0.4,  PARALLEL      );
  // turnInPlace();
  motors->setPower(0,0);
  while(true){
    Serial.println("we fuckin done boiiiiiiiiiiii");
    delay(1000);
  }
}

void Sorry::drive(int num, int segment, float motorPower, unsigned long timeout, float desiredDistToLeftWall, CorrectionMode correctionMode, float distanceToStopAt, float pitchToStopAt, bool isMinimumPitchThreshold){
  Serial.printf("\n-----------------------------------------------\nbegin drive %d.%d @pow=%3.0f for t=%lu, desiredLeftDist=%3.2f, correctionMode=%d, stop@=%3.2f, stopPitch=%5.3f\n",
      num, segment, motorPower, timeout, desiredDistToLeftWall, correctionMode, distanceToStopAt, pitchToStopAt);
  timeout *= 1000;
  startCurDriveSegmentT = micros();
  curT = micros();
  firstTick = true;
  while(true){
    deltaT = micros() - curT;
    curT += deltaT;
    float frontDist = getTofFt(0);
    float pitch = sensors->getGyroAnglePitch();
    if (distanceToStopAt > 0){
      Serial.printf("frontD: %5.4f distToStop: %5.4f\n", frontDist, distanceToStopAt);
      if (frontDist <= distanceToStopAt + STOP_OFFSET){
        break;
      }
    }
    if (pitchToStopAt != NO_PITCH_ANGLE_THRESHOLD){
      Serial.printf("Gyro pitch: %5.4f pitchToStop: %5.4f\n", pitch, pitchToStopAt);
      if (isMinimumPitchThreshold){
        if (pitch > 0){
          if (pitch <= pitchToStopAt){
            Serial.println("met pitch min threshold, breaking segment");
            break;
          }
        }
        else{
        }
      }
      else{
        if (pitch > 0){
        }
        else{
          if (pitch >= pitchToStopAt){
            Serial.println("met pitch max threshold, breaking segment");
            break;
          }
        }
      }
    }
    if (curT - startCurDriveSegmentT >= timeout){
      break;
    }
    driveTick(motorPower, desiredDistToLeftWall, correctionMode, firstTick);
    firstTick = false;
  }

  if (motorPower == STOPPED_POWER){
    motors->setAllToZero();
  }
}

bool Sorry::updateWallAngleAndDistance(float gyroAngle, float desiredDistToLeftWall, bool firstTick){
  if (!(isValid(1) && isValid(2))){
    return false;
  }
  angFromWall = rad2deg(atan((getTofFt(1) - getTofFt(2)) / L_Y_DELTA));

  curDistToLeftWall = ((getTofFt(1) + getTofFt(2)) / 2 + L_X_OFFSET) * cosd(angFromWall);
  /* if (abs(curDistToLeftWall - desiredDistToLeftWall) > MAX_TOLERABLE_WALL_DISTANCE_DIVERGENCE){ */
    /* return false; */
  /* } */
  return true;

  /* if (firstTick){ */
    /* return true; */
  /* } */
  /* return abs(abs(gyroAngle - angFromWall) - abs(gyroAngleAtLastValidWallAngle - lastValidWallAngle)) < MAXIMUM_WALL_ANGLE_DIVERGENCE_BEFORE_DISTRUSTING_LEFT_TOFS; */
}

float Sorry::getDirectionCorrectedGyroAngle(){
  return bringGyroMeasurementIntoPositiveDegreesUsingNumClockwiseWraparounds(sensors->getGyroAngle());
}

// correct a gyro angle 
float Sorry::getDriftCorrectedGyroAngle(unsigned long microsSinceLastZeroed){
  float directionCorrectedAngle = getDirectionCorrectedGyroAngle();
  float driftCorrectedAngle = directionCorrectedAngle - microsSinceLastZeroed * gyroDriftPerMicro;
  return driftCorrectedAngle;
}

float Sorry::bringGyroMeasurementIntoPositiveDegreesUsingNumClockwiseWraparounds(float rawAngle){
  /* Serial.println("bringGyro"); */
  float correctedAngle = rawAngle;
  if (rawAngle < 0 && prevGyroMeasurementWasLargeAndMightWrapAroundToNegative){
    correctedAngle += 360;
    numGyroClockwiseWraparounds++;
  }
  correctedAngle += 360 * numGyroClockwiseWraparounds;
  prevGyroMeasurementWasLargeAndMightWrapAroundToNegative = rawAngle > 150;
  /* Serial.print("bring to positive -> rawAngle: "); Serial.print(rawAngle); Serial.print(" +360*n: "); Serial.println(correctedAngle); */
  /* Serial.println("bringGyro done"); */
  return correctedAngle;
}

void Sorry::calibrateGyroDrift(){
  unsigned long preCalibrationT;

  // start by waiting for gyro to calm the fuck down because when it first inits it sucks dick
  Serial.println("getting a prevAngle");
  float prevAngle = getDirectionCorrectedGyroAngle();
  Serial.println("got a prevAngle");
  unsigned long lastTimestamp = micros();
  delay(5);
  int numSatisfactoryTicks = 0; // number of ticks where gyro drift is slow enough
  Serial.println("start loop 1");
  int dupeAngleCount = 0;
  while(micros()-preCalibrationT<MAX_WAIT_FOR_DUMB_GYRO_SECONDS*1000000){
    delay(5);
    float curAngle = getDirectionCorrectedGyroAngle();
    if (curAngle == prevAngle){
      dupeAngleCount++;
      if (dupeAngleCount % 10 == 0){
        while(true){
          Serial.println("Gyro read the same value twice, probably broken because it should be drifting rn!!!!!");
          motors->setPower(40,40);
          delay(1000);
          motors->setPower(0,0);
          delay(1000);
        }
      }
    }
    else{
      dupeAngleCount = 0;
    }
    unsigned long curTimestamp = micros();
    unsigned long deltaMicros = curTimestamp - lastTimestamp;
    Serial.print("deltaMicros: "); Serial.println(deltaMicros);
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
    Serial.printf("precal. ang: %6.3f\n");
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


bool Sorry::isValid(int tofNum){
  return !(getTofFt(tofNum) > TRACK_DIM  || getTofFt(tofNum) <= 0);
}

float Sorry::getTofFt(int tofNum){
  return sensors->tof[tofNum].getData().dist * 0.00328084; //mm -> ft
}

/* void Sorry::driveTick(float motorPower, float leftWallDist){ */
void Sorry::driveTick(float motorPower, float desiredDistToLeftWall, CorrectionMode correctionMode, bool firstTick){
  sensors->update();
  float gyroAngle = sensors->getGyroAngle();

  // DRIFT PID --------------------------------------------------------------------------
  /* Serial.printf("pitch angle: %6.3f", sensors->getGyroAnglePitch()); */
  float lastAngleError = angleError;

  bool updatedAngleAndDistSucc = updateWallAngleAndDistance(gyroAngle, desiredDistToLeftWall, firstTick);
  if (!updatedAngleAndDistSucc || correctionMode == UNGUIDED){
    Serial.println("unguided or invalid left data");
    motors->setPower(motorPower, motorPower*LOWER_RIGHT_VEL_SP_BY);
    return;
  }
  float distToLeftWallError = curDistToLeftWall - desiredDistToLeftWall;
  float desiredAngle = -rad2deg(atan(distToLeftWallError/DRIFT_LOOK_AHEAD_DIST));

  if (correctionMode == PARALLEL){
    desiredAngle = 0;
  }

  angleError = angFromWall - desiredAngle;
  float angleErrorD = (angleError - lastAngleError)*1000/deltaT;
  angleErrorI += angleError * deltaT/1000;
  if (sign(lastAngleError) != sign(angleError)){
    angleErrorI = 0;
  }
  angleErrorI = constrainVal(angleErrorI, MAX_DRIFT_ERROR_I);

  float P = angleError * kP_drift;
  float I = angleErrorI * kI_drift;
  float D = angleErrorD * kD_drift;

  float driftOutput = P + I + D;
  if (motorPower == SLOW_POWER){
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

  if (leftTotal < 0){
    leftTotal = 0; 
  }
  if (rightTotal < 0){
    rightTotal = 0;
  }
  Serial.printf("ang=%7.3f desired=%7.3f lDr=%4.1f rDr=%4.1f dt=%7.3f bat=%7.3f\n",
        angFromWall, desiredAngle, leftOutputDrift, rightOutputDrift,
        hms->data.batteryVoltage);
  motors->setPower(leftTotal, rightTotal*LOWER_RIGHT_VEL_SP_BY);
}

void Sorry::turnInPlace(int turnNum){
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
  Serial.printf("Start turn #%d\n",turnNum);
  float startAngle = getDirectionCorrectedGyroAngle();
  float curAngle = startAngle;

  while(true){
    curTs = micros();
    curAngle = getDriftCorrectedGyroAngle(curTs - _firstT);
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
