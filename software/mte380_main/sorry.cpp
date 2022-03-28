#include "sorry.h"
#include "math_utils.h"

#define ULTRA_ULTRA_POWER 180
#define ULTRA_POWER 140
#define FAST_POWER 80
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

Sorry::Sorry(Motors* motors, Sensors* sensors, Nav* nav, Hms* hms):
motors(motors),
sensors(sensors),
hms(hms),
nav(nav){
  errDriftI = 0;
  errDrift = 0;
  angFromWall = 0;
  deltaT = 0;
  curT = micros();
}

void Sorry::run(){
  /* calibrateGyro(); */
  drive(FAST_POWER,    800,  0.5,  GUIDED        );
  drive(MEDIUM_POWER,  200,  0.5,  GUIDED        );
  drive(SLOW_POWER,    2000, 0.5,  GUIDED,   0.55);
  drive(STOPPED_POWER, 500,  0.5,  GUIDED        );
  turnInPlace();
  drive(FAST_POWER,    800,  0.5,  GUIDED        );
  drive(MEDIUM_POWER,  200,  0.5,  GUIDED        );
  drive(SLOW_POWER,    2000, 0.5,  GUIDED,   0.55);
  drive(STOPPED_POWER, 500,  0.5,  GUIDED        );
  turnInPlace();
  drive(FAST_POWER,    800,  0.5,  GUIDED        );
  drive(MEDIUM_POWER,  200,  0.5,  GUIDED        );
  drive(SLOW_POWER,    2000, 0.5,  GUIDED,   0.48);
  drive(STOPPED_POWER, 500,  0.5,  PARALLEL      );
  turnInPlace();
  drive(MEDIUM_POWER,  3300, 0.4,  GUIDED        );
  drive(ULTRA_POWER,   260,  0.4,  UNGUIDED      );
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

void Sorry::drive(float motorPower, unsigned long timeout, float leftWallDist, CorrectionMode correctionMode, float distanceToStopAt){
  timeout *= 1000;
  unsigned long startT = micros();
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
    if (curT - startT >= timeout){
      break;
    }
    nav->getGyroAngle(); // literally just so fusion updates
    driveTick(motorPower, leftWallDist, correctionMode, firstTick);
    firstTick = false;
  }

  if (motorPower == STOPPED_POWER){
    motors->setAllToZero();
  }
}

bool Sorry::isValid(int tofNum){
  return !(getTofFt(tofNum) > TRACK_DIM  || getTofFt(tofNum) <= 0);
}

float Sorry::getTofFt(int tofNum){
  return sensors->tof[tofNum].getData().dist * 0.00328084; //mm -> ft
}

bool Sorry::updateWallAngleAndDistance(){
  angFromWall = rad2deg(atan((getTofFt(1) - getTofFt(2)) / L_Y_DELTA));
  left = ((getTofFt(1) + getTofFt(2)) / 2 + L_X_OFFSET) * cosd(angFromWall);
  return isValid(1) && isValid(2);
}

void Sorry::driveTick(float motorPower, float leftWallDist, CorrectionMode correctionMode, bool firstTick){
  if (correctionMode != GUIDED_GYRO){
    sensors->update();
  }
  float gyroAngle = nav->getGyroAngle();

  // DRIFT PID --------------------------------------------------------------------------
  float lastErrDrift = errDrift;
  float dist = left - leftWallDist;

  float desiredAngle;
  if (!updateWallAngleAndDistance() || correctionMode == UNGUIDED){
    motors->setPower(motorPower, motorPower);
    return;
  }
  else if (correctionMode == PARALLEL){
    desiredAngle = 0;
  }
  else if (correctionMode == GUIDED){
    desiredAngle = -rad2deg(atan(dist/DRIFT_LOOK_AHEAD_DIST));
  }
  errDrift = angFromWall - desiredAngle;
  if (correctionMode == GUIDED_GYRO){
    if (firstTick){
      gyroTurnStartAngle = gyroAngle;
    }
    else{
      desiredAngle = gyroTurnStartAngle;
      errDrift = gyroAngle - desiredAngle;
    }
  }
  float errDriftD = (errDrift - lastErrDrift)*1000/deltaT;
  errDriftI += errDrift * deltaT/1000;
  if (sign(lastErrDrift) != sign(errDrift)){
    errDriftI = 0;
  }
  if (firstTick){
    errDriftI = 0;
    errDriftD = 0;
  }
  errDriftI = constrainVal(errDriftI, MAX_DRIFT_ERROR_I);

  float P = errDrift * kP_drift;
  float I = errDriftI * kI_drift;
  float D = errDriftD * kD_drift;

  float driftOutput = P + I + D;

  driftOutput *= motorPower / MAX_OUTPUT_POWER;
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
  Serial.printf("ang=%7.3f desired=%7.3f lDr=%4.1f rDr=%4.1f dt=%7.3f bat=%7.3f\n",
        angFromWall, desiredAngle, leftOutputDrift, rightOutputDrift,
        hms->data.batteryVoltage);
  motors->setPower(leftTotal, rightTotal*LOWER_RIGHT_VEL_SP_BY);
  nav->getGyroAngle();
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
  float startAngle = nav->getGyroAngle();
  float curAngle = startAngle;

  while(true){
    rawAngle = nav->getGyroAngle();
    if (curAngle - rawAngle > 300){ //300 since cur - new will loop over to 360 degrees, but not quite 360
      curAngle = 360 + rawAngle;
    }
    else{
      curAngle = rawAngle;
    }
    angleDelta = curAngle - startAngle;
    error = turnAmount - angleDelta;
    curTs = micros();
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
    Serial.printf("StartAngle: %.3f | rawAngle: %.3f | curAngle(adj): %.3f | P: %.3f * %.3f = %.3f D: %.3f * %.3f = %.3f | I: %.3f * %.3f = %.3f | L: %.3f, R: %.3f\n", startAngle, rawAngle, curAngle, error,kp_turny,P, errorD,kd_turny,D, errorI, ki_turny, I, total, -total);
    motors->setPower(total, -total*LOWER_RIGHT_VEL_SP_BY);

    lastError = error;

  }
  motors->setAllToZero();
  /* while(true){ */
  /* } */
  return;
}
