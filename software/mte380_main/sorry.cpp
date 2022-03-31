#include "sorry.h"
#include "math_utils.h"

#define ULTRA_POWER_MWAHAHAHA 110
#define MOUNT_WALL_POWER 95
#define FAST_POWER 62
#define MEDIUM_POWER 62
#define SLOW_POWER 62
/* #define MEDIUM_POWER 55 */
/* #define SLOW_POWER 44 */

#define MAX_OUTPUT_POWER 89

#define TRACK_DIM 6  // ft
#define L_Y_DELTA 0.3958  // ft
#define L_X_OFFSET 0.213  // ft

#define MAX_TURN_IN_PLACE_OUTPUT_POWER 60//75 // must be < 255
#define MAX_TURN_IN_PLACE_ERROR_I 1200
#define MAX_DRIFT_ERROR_I 200

#define LOWER_RIGHT_VEL_SP_BY 0.87
#define DRIFT_LOOK_AHEAD_DIST 1.5

#define STOP_OFFSET 0.46
#define REVERSE_SPEED -40
#define REVERSE_TIME 1000

#define MAX_TURN_IN_PLACE_ERROR_I 700

/* #define kP_drift 0.55 */
/* #define kD_drift 900 */
/* #define kI_drift 0.002 */
#define kP_drift 0.7
#define kD_drift 900
#define kI_drift 0.0//

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
  dontCorrectDrift = false;
}

void Sorry::run(){
  drive(0, 800000, 0, 200000, 0.55, 0.5);
  turnInPlace();
  drive(0, 800000, 0, 200000, 0.55, 0.5);
  turnInPlace();
  drive(0, 800000, 0, 200000, 0.53, 0.5);
  turnInPlace();
  drive(2400000, 0, 0, 0, 1.5, 0.4);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 1.35, 1.47);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 1.5, 1.47);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 1.2, 1.47);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 1.5, 1.47);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 2.5, 1.47);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 2.5, 2.47);
  turnInPlace();
  drive(0, 1000000, 0, 1200000, 2.5, 2.47);
  turnInPlace();
  drive(0, 500000, 0, 500000, 2.5, 2.47);
}

void Sorry::drive(unsigned long goMountSpeedForFirst, unsigned long goFastFor, unsigned long goFastUnguidedDur, unsigned long goMediumFor, float distanceToStopAt, float leftWallDist, bool ultraPower){
  if (goMountSpeedForFirst > 0){
    unsigned long startT0 = micros();
    while(true){
      deltaT = micros() - curT;
      curT += deltaT;
      if (curT - startT0 >= goMountSpeedForFirst){
        break;
      }
      driveTick(MOUNT_WALL_POWER, leftWallDist);
    }
  }
  if (goFastFor > 0){
    unsigned long startT1 = micros();
    while(true){
      deltaT = micros() - curT;
      curT += deltaT;
      if (curT - startT1 <= goFastFor - goFastUnguidedDur){
        dontCorrectDrift = true;
      }
      else{
        dontCorrectDrift = false;
      }
      if (curT - startT1 >= goFastFor){
        break;
      }
      driveTick(ultraPower?ULTRA_POWER_MWAHAHAHA:FAST_POWER, leftWallDist);
    }
    dontCorrectDrift = false;
  }

  if (goMediumFor > 0){
    unsigned long startT2 = micros();
    /* motors->setPower(MEDIUM, MEDIUM); */
    while(true){
      deltaT = micros() - curT;
      curT += deltaT;
      if (curT - startT2 >= goMediumFor){
        break;
      }
      driveTick(MEDIUM_POWER, leftWallDist);
    }
  }

  unsigned long startLastMoveT = micros();
  unsigned long timeout = 8 * 1000000;
  while(true){
    deltaT = micros() - curT;
    curT += deltaT;
    float frontDist = getTofFt(0);
    Serial.printf("frontD: %5.4f distToStop: %5.4f\n", frontDist, distanceToStopAt);
    if (frontDist <= distanceToStopAt + STOP_OFFSET){
      break;
    }
    if (micros() - startLastMoveT >= timeout){
      break;
    }
    nav->getGyroAngle(); // literally just so fusion updates
    driveTick(SLOW_POWER, leftWallDist);
  }
  unsigned long startStayStillT = micros();
  unsigned long stayStillTime = 1000000;
  curT = micros();
  while (micros() - startStayStillT < stayStillTime){
    deltaT = micros() - curT;
    curT += deltaT;
    float frontDist = getTofFt(0);
    Serial.printf("xxfrontD: %5.4f distToStop: %5.4f\n", frontDist, distanceToStopAt);
    nav->getGyroAngle(); // literally just so fusion updates
    driveTick(0, leftWallDist);
  }
  motors->setAllToZero();
  unsigned long beforeFinishT = micros();
  unsigned long endWait = 1000000;
  while(micros()-beforeFinishT < endWait){
    Serial.println(nav->getGyroAngle());
    delay(40);
  }

  // RESET INTEGRAL TERMS!!
  errDriftI = 0;
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

void Sorry::driveTick(float motorPower, float leftWallDist){
  sensors->update();
  nav->getGyroAngle();
  // DRIFT PID --------------------------------------------------------------------------
  Serial.printf("pitch angle: %6.3f", sensors->getGyroAnglePitch());
  float lastErrDrift = errDrift;
  float dist = left - leftWallDist;

  float desiredAngle = -rad2deg(atan(dist/DRIFT_LOOK_AHEAD_DIST));

  if (!updateWallAngleAndDistance() || (motorPower == ULTRA_POWER_MWAHAHAHA && dontCorrectDrift)){
    motors->setPower(motorPower, motorPower);
  }

  errDrift = angFromWall - desiredAngle;
  float errDriftD = (errDrift - lastErrDrift)*1000/deltaT;
  errDriftI += errDrift * deltaT/1000;
  if (sign(lastErrDrift) != sign(errDrift)){
    errDriftI = 0;
  }
  errDriftI = constrainVal(errDriftI, MAX_DRIFT_ERROR_I);

  float P = errDrift * kP_drift;
  float I = errDriftI * kI_drift;
  float D = errDriftD * kD_drift;

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
  nav->getGyroAngle();
}

void Sorry::turnInPlace(){
  // float threshhold = 5; // end loop when 5 degrees from donezo
  float threshold = 3; // end loop when 2 degrees from donezo for thresholdTime sec
  unsigned long thresholdTime = 50000;
  float angleDelta = 0;

  float DUMB_ERROR_OFFSET = 0;

  float turnAmount = 90 + DUMB_ERROR_OFFSET;
  float error = turnAmount;
  float lastError = error;
  float kp_turny = 1.5;
  float kd_turny = 400;
  float ki_turny = 0.18 * (hms->data.nCells < 3 ? 1.5: 1);
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
    if (curTs - _firstT > 3*1000*1000){
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
    if (total < 0){
      total = 0;
    }
    Serial.printf("StartAngle: %.3f | rawAngle: %.3f | curAngle(adj): %.3f | P: %.3f * %.3f = %.3f D: %.3f * %.3f = %.3f | I: %.3f * %.3f = %.3f | L: %.3f, R: %.3f\n", startAngle, rawAngle, curAngle, error,kp_turny,P, errorD,kd_turny,D, errorI, ki_turny, I, total, -total);
    motors->setPower(total, -total*LOWER_RIGHT_VEL_SP_BY);

    lastError = error;

  }
  motors->setAllToZero();
  /* while(true){ */
  /* } */
  return;
}
