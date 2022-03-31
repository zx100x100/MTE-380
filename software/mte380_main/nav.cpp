#include "nav.h"
#include "math_utils.h"

#define L_Y_DELTA 0.3958  // ft
#define L_X_OFFSET 0.213  // ft
#define F_Y_OFFSET 0.246  // ft
#define F_X_OFFSET 0.156  // ft
#define B_Y_OFFSET 0.246  // ft
#define B_X_OFFSET 0.156  // ft

#define TRACK_DIM 6  // ft
#define MAX_DEVIATION 1  // ft

#define USE_IMU false
#define USE_TOFS true

#define STARTING_X 4.5
#define STARTING_Y 5.5
#define STARTING_YAW 180

#define FLOAT_INVALID 0xFFFF // TODO: make this live in globals .h

Nav::Nav(Sensors& sensors, CmdData* cmdData, Hms* hms):
  sensors(sensors),
  cmdData(cmdData),
  hms(hms)
{
  navData = NavData_init_zero;
  navData.posX = STARTING_X;
  navData.posY = STARTING_Y;
  navData.angXy = STARTING_YAW;
  tofPos.x = navData.posX;
  tofPos.y = navData.posY;
  tofPos.yaw = navData.angXy;
  lastBackSensorVal = 0;
}

void Nav::init(){
  //set up sensor fusion lib
  // fusion.setup(sensors.imu.getData().accelX, sensors.imu.getData().accelY, sensors.imu.getData().accelZ);

  unsigned long startT = micros();
  unsigned long curT = 0;
  unsigned long initTime = 1*1000*1000;
  Serial.println("waiting for gyro to calm down");
  while(true){
    float rawAngle = getGyroAngle();
    Serial.print("gyro raw (stop drifting pls): "); Serial.println(rawAngle);
    curT = micros();
    if (curT - startT > initTime){
      break;
    }
  }
}

float Nav::getTofFt(TofOrder tofNum){
    return sensors.tof[tofNum].getData().dist * 0.00328084; //mm -> ft
}

NavData Nav::calculateImu(){
  return NavData_init_zero;
}

float Nav::getGyroAngle(){
  sensors.imu.poll();

  fusion.update(sensors.imu.getData().gyroX, sensors.imu.getData().gyroY, sensors.imu.getData().gyroZ, sensors.imu.getData().accelX, sensors.imu.getData().accelY, sensors.imu.getData().accelZ);

  float yaw = -rad2deg(fusion.yaw());
  // float yaw = 5;
  /* Serial.print("yaw: "); Serial.println(yaw); */

  return yaw;
}

bool Nav::tofsUpdated(){
    bool changed = true;
    for (int i = 0; i < 4; ++i){
        changed &= sensors.tof[i].getData().count != lastTofsCount[i];
    }
    if (changed){
        for (int i = 0; i < 4; i++){
            lastTofsCount[i] = sensors.tof[i].getData().count;
        }
    }
    return changed;
}

float Nav::predToFront(GuidanceData_Heading heading){
    switch(heading){
      case GuidanceData_Heading_UP:
        return predNavData.posY;
        break;
      case GuidanceData_Heading_RIGHT:
        return TRACK_DIM - predNavData.posX;
        break;
      case GuidanceData_Heading_DOWN:
        return TRACK_DIM - predNavData.posY;
        break;
      case GuidanceData_Heading_LEFT:
        return predNavData.posX;
        break;
      default:
        return FLOAT_INVALID;
    }
}

void Nav::updateTof(GuidanceData_Heading heading){
  if (tofsUpdated()){
      float angFromWall, front, left;
      bool angFromWallValid, frontValid, leftValid;

      if (hms->data.navLogLevel >= 2) Serial.println("tofs updated");
      angFromWall = rad2deg(atan((getTofFt(L_FRONT) - getTofFt(L_BACK)) / L_Y_DELTA));
      // The following assumes L_BACK and L_FRONT symmetrical about center of beep boop
      left = ((getTofFt(L_FRONT) + getTofFt(L_BACK)) / 2 + L_X_OFFSET) * cosd(angFromWall);
      if(hms->data.sensorsLogLevel >= 1){ Serial.print("isValid(L_FRONT): "); Serial.println(isValid(L_FRONT)); }
      if(hms->data.sensorsLogLevel >= 1){ Serial.print("isValid(L_BACK): "); Serial.println(isValid(L_BACK)); }
      if (isValid(L_FRONT) && isValid(L_BACK)) {//TODO && (fmod(navData.angXy, 90) < 35 || fmod(navData.angXy, 90) > 55)) {
        angFromWallValid = true;
        leftValid = true;
        if (hms->data.navLogLevel >= 2) Serial.println("Left valid");
      }
      else{
        if (hms->data.navLogLevel >= 2) Serial.println("Left invalid");
        angFromWallValid = false;
        leftValid = false;
      }
      
      float estimateFromFrontSensor;
      if(isValid(FRONT)){
        estimateFromFrontSensor = (getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall);
      }
      else{
        estimateFromFrontSensor = FLOAT_INVALID;
      }
      
      float estimateFromBackSensor;
      if (isValid(BACK)){
        estimateFromBackSensor = TRACK_DIM - ((getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall));
      }
      else{
        estimateFromBackSensor = FLOAT_INVALID;
      }
      float estimateSensorAvg = (estimateFromFrontSensor + estimateFromBackSensor) / 2;
            
      float estimateToWall = estimateSensorAvg;
      /* if (fabs(predToFront(heading) - estimateFromFrontSensor) < fabs(predToFront(heading) - estimateToWall)) { */
      bool backFrozen = getTofFt(BACK) == lastBackSensorVal;
      bool justUseFront = (predToFront(heading) < 1.5 && estimateFromFrontSensor < 1.5) || backFrozen || getTofFt(BACK) > 4.0;
      if (!justUseFront && angFromWallValid && fabs(angFromWall)<10 && getTofFt(FRONT) < 2){
        justUseFront = true;
      }
      if(hms->data.sensorsLogLevel >= 1){ Serial.print("estimateFromFrontSensor: "); Serial.println(estimateFromFrontSensor); }
      if(hms->data.sensorsLogLevel >= 1){ Serial.print("predToFront(heading): "); Serial.println(predToFront(heading)); }
      if ((fabs(predToFront(heading) - estimateFromFrontSensor) < fabs(predToFront(heading) - estimateToWall))
      || justUseFront){
        estimateToWall = estimateFromFrontSensor;
        if (justUseFront){
          Serial.println("JUST using front sensor!!!");
        }
        else{
          Serial.println("using front sensor!!!");
        }
      }
      if ((fabs(predToFront(heading) - estimateFromBackSensor) < fabs(predToFront(heading) - estimateToWall))
          && !justUseFront) {
        Serial.println("using back sensor");
        estimateToWall = estimateFromBackSensor;
      }
      
      front = estimateToWall;
      if (fabs(predToFront(heading) - estimateToWall) < MAX_DEVIATION){  // if our best guess is good
        frontValid = true;
      }
      else{ // our best guess sucks. Tof luck bud
        if (hms->data.navLogLevel >= 1) Serial.println("vertical tofs INVALID");
        if (justUseFront){
          if (hms->data.navLogLevel >= 1) Serial.println("jk were gonna use em anyway cuz justUseFront==true");
          frontValid = true;
        }
        else{
          frontValid = false;
        }
      }

    /* if(hms->data.navLogLevel >= 1){ Serial.print("Heading: "); Serial.println(heading); } */
    tofPos.yawValid = angFromWallValid;  // TODO: validate angle change
    
    // Local to global coordinate conversion switch case:
    navData.left = left;
    navData.front = front;
    navData.angFromWall = angFromWall;
    switch(heading){
      case GuidanceData_Heading_UP:
        if(hms->data.sensorsLogLevel >= 1){ Serial.print("fabs(tofPos.x - left) < MAX_DEVIATION: "); Serial.println(fabs(tofPos.x - left) < MAX_DEVIATION); }
        if(hms->data.sensorsLogLevel >= 1){ Serial.print("leftValid: "); Serial.println(leftValid); }
        tofPos.xValid = leftValid && fabs(tofPos.x - left) < MAX_DEVIATION;
        tofPos.yValid = frontValid && fabs(tofPos.y - front) < MAX_DEVIATION;
        navData.xValid = tofPos.xValid;
        navData.yValid = tofPos.yValid;
        /* if (tofPos.xValid) tofPos.x = left; */
        /* if (tofPos.yValid) tofPos.y = front; */
        tofPos.x = left;
        tofPos.y = front;
        if (angFromWallValid) tofPos.yaw = angFromWall + 270;

        break;
      case GuidanceData_Heading_RIGHT:
        tofPos.xValid = frontValid && fabs(tofPos.x - (TRACK_DIM - front)) < MAX_DEVIATION;
        tofPos.yValid = leftValid && fabs(tofPos.y - left) < MAX_DEVIATION;
        navData.xValid = tofPos.xValid;
        navData.yValid = tofPos.yValid;
        /* if (tofPos.xValid) tofPos.x = TRACK_DIM - front; */
        /* if (tofPos.yValid) tofPos.y = left; */
        tofPos.x = TRACK_DIM - front;
        tofPos.y = left;
        tofPos.x = TRACK_DIM - front;
        tofPos.y = left;
        if (angFromWallValid) tofPos.yaw = angFromWall;
        break;
      case GuidanceData_Heading_DOWN:
        tofPos.xValid = leftValid && fabs(tofPos.x - (TRACK_DIM - left)) < MAX_DEVIATION;
        tofPos.yValid = frontValid && fabs(tofPos.y - (TRACK_DIM - front)) < MAX_DEVIATION;
        navData.xValid = tofPos.xValid;
        navData.yValid = tofPos.yValid;
        /* if (tofPos.xValid) tofPos.x = TRACK_DIM - left; */
        /* if (tofPos.yValid) tofPos.y = TRACK_DIM - front; */
        tofPos.x = TRACK_DIM - left;
        tofPos.y = TRACK_DIM - front;
        if (angFromWallValid) tofPos.yaw = angFromWall + 90;
        break;
      case GuidanceData_Heading_LEFT:
        tofPos.xValid = frontValid && fabs(tofPos.x - front) < MAX_DEVIATION;
        tofPos.yValid = leftValid && fabs(tofPos.y - (TRACK_DIM - left)) < MAX_DEVIATION;
        navData.xValid = tofPos.xValid;
        navData.yValid = tofPos.yValid;
        /* if (tofPos.xValid) tofPos.x = front; */
        /* if (tofPos.yValid) tofPos.y = TRACK_DIM - left; */
        tofPos.x = front;
        tofPos.y = TRACK_DIM - left;
        if (angFromWallValid) tofPos.yaw = angFromWall + 180;
        break;
    }
    if (hms->data.navLogLevel >= 1){
      /* Serial.printf("xValid: %d x: %7.3f yValid: %d y: %7.3f\n", tofPos.xValid, tofPos.x, tofPos.yValid, tofPos.y); */
    }
    lastBackSensorVal = getTofFt(BACK);
  }
  else{ // ToFs not updated
    if (hms->data.navLogLevel >= 2) Serial.println("tofs NOT updated");
    tofPos.xValid = false;
    tofPos.yValid = false;
    navData.xValid = tofPos.xValid;
    navData.yValid = tofPos.yValid;
    tofPos.yawValid = false;
  }
}

void Nav::update(GuidanceData_Heading heading){
  Serial.println("NAV UPDATE------");
//  if(hms->data.guidanceLogLevel >= 2){ Serial.println("nav update"); } // temp
  if (cmdData -> runState == CmdData_RunState_SIM){
//    if(hms->data.guidanceLogLevel >= 2){ Serial.println("nav in SIM mode"); }
//    if(hms->data.guidanceLogLevel >= 2){ Serial.print("cmdData->simPosX: "); Serial.println(cmdData->simPosX); }
    navData.posX = cmdData->simPosX;
    navData.posY = cmdData->simPosY;
    navData.posZ = 0;
    navData.velX = cmdData->simVelX;
    navData.velY = cmdData->simVelY;
    navData.velZ = 0;
    navData.accX = cmdData->simAccX;
    navData.accY = cmdData->simAccY;
    navData.accZ = 0;
    navData.angXy = cmdData->simAngXy;
    navData.angVelXy = cmdData->simAngVelXy;
    navData.angVelXz = 0;
    navData.angVelYz = 0;
    navData.angAccXy = cmdData->simAngAccXy;
    navData.angAccXz = 0;
    navData.angAccYz = 0;
    navData.timestamp = micros();
    return;
  }
  else{
//    if(hms->data.guidanceLogLevel >= 2){ Serial.println("NAV::::::::: NOT IN SIM MODE??????"); }
    // return;
  }

  updatePred();
  getGyroAngle(); // TODO: Fuse Gyro angle with TOF angle and decide when to use each one!

  updateTof(heading);
  /* if (hms->data.navLogLevel >= 1){ */
    /* Serial.print("tof: FR: "); Serial.print(getTofFt(FRONT)); Serial.print(", LF: ");  Serial.print(getTofFt(L_FRONT)); Serial.print(", LB: ");  Serial.print(getTofFt(L_BACK)); Serial.print(", BA: ");  Serial.println(getTofFt(BACK)); */
    /* Serial.print("tof estimate: x: "); Serial.print(tofPos.x); Serial.print(", left: ");  Serial.print(tofPos.y); Serial.print(", yaw: ");  Serial.println(tofPos.yaw); */
  /* } */

  updateEstimate(predNavData);
  if (hms->data.navLogLevel >= 1){
    /* Serial.print("estimate: x: "); Serial.print(navData.posX); Serial.print(", y: ");  Serial.print(navData.posY); Serial.print(", yaw: ");  Serial.println(navData.angXy); */
  }
  if (hms->data.navLogLevel >= 1){
    Serial.print("predNavData: x: "); Serial.print(predNavData.posX); Serial.print(", y: ");  Serial.print(predNavData.posY); Serial.print(", yaw: ");  Serial.println(predNavData.angXy);
    Serial.printf("predX: %3.2f predY: %3.2f  yaw: %4.1f tof1: %4f tof2: %4f tof3: %4f tof4: %4f tofX: %3.2f tofY: %3.2f tofYaw: %4.1f estX: %3.2f estY: %3.2f estYaw: %4.1f xV: %d, yV: %d\n",
        predNavData.posX,predNavData.posY, predNavData.angXy,
        getTofFt(FRONT),getTofFt(L_FRONT),getTofFt(L_BACK),getTofFt(BACK),
        tofPos.x, tofPos.y, tofPos.yaw,
        navData.posX, navData.posY, navData.angXy,
        tofPos.xValid, tofPos.yValid);
  }

}

NavData& Nav::getData(){
  return navData;
}

bool Nav::isValid(TofOrder tof){
  if (getTofFt(tof) > TRACK_DIM  || getTofFt(tof) <= 0)
    return false;
  return true;
//
//  float angFromWall;
//  if (fmod(navData.angXy, 90) < 45){ // angFromWall should be > 0
//    angFromWall = fmod(navData.angXy, 90);
//  }
//  else{  // angFromWall should be < 0
//    angFromWall = fmod(navData.angXy, 90) - 90;
//  }
//
//  float estimateLeft, estimateFront;
//  switch(int(round(navData.angXy / 90)) % 4){   //Global to Local
//    case 0:
//      estimateLeft = navData.posY;
//      estimateFront = TRACK_DIM - navData.posX;
//      break;
//    case 1:
//      estimateLeft = TRACK_DIM - navData.posX;
//      estimateFront = TRACK_DIM - navData.posY;
//      break;
//    case 2:
//      estimateLeft = TRACK_DIM - navData.posY;
//      estimateFront = navData.posX;
//      break;
//    case 3:
//      estimateLeft = navData.posX;
//      estimateFront = navData.posY;
//  }
//
//  switch (tof){
//    case FRONT:
//      return estimateFront - ((getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall)) <= MAX_DEVIATION;
//      break;
//    case L_FRONT:
//      return estimateLeft - ((getTofFt(L_FRONT) + L_X_OFFSET) * cosd(angFromWall) - L_Y_DELTA / 2 * sin(angFromWall)) <= MAX_DEVIATION;
//      break;
//    case L_BACK:
//      return estimateLeft - ((getTofFt(L_BACK) + L_X_OFFSET) * cosd(angFromWall) + L_Y_DELTA / 2 * sind(angFromWall)) <= MAX_DEVIATION;
//      break;
//    case BACK:
//      return estimateFront - (TRACK_DIM - (getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall)) <= MAX_DEVIATION;
//  }
}

void Nav::updatePred(){
  float delT = float(sensors.timestamp - navData.timestamp) / 1000000; // navData timestamp is the same as previous sensor timestamp
  predNavData.angAccXy = navData.angAccXy;
  predNavData.angAccXz = navData.angAccXz;
  predNavData.angAccYz = navData.angAccYz;
  predNavData.angVelXy = navData.angVelXy;
  predNavData.angVelXz = navData.angVelXz;
  predNavData.angVelYz = navData.angVelYz;
  predNavData.angXy = navData.angXy;
  predNavData.angXz = navData.angXz;
  predNavData.angYz = navData.angYz;  // TODO: do we just want it to equal the previous one?

  // TODO: take rotation into account
  predNavData.accX = navData.accX;
  predNavData.accY = navData.accY;
  predNavData.accZ = navData.accZ;
  predNavData.velX = navData.velX; // + navData.accX * delT;
  predNavData.velY = navData.velY; // + navData.accY * delT;
  predNavData.velZ = navData.velZ; // + navData.accZ * delT;
  predNavData.posX = navData.posX; // + navData.velX * delT; // + navData.accX * delT * delT / 2;
  predNavData.posY = navData.posY; // + navData.velY * delT; // + navData.accY * delT * delT / 2;
  predNavData.posZ = navData.posZ; // + navData.velZ * delT; // + navData.accZ * delT * delT / 2;

  predNavData.timestamp = sensors.timestamp;
}

void Nav::updateEstimate(const NavData predNavData){
  float delT = float(predNavData.timestamp - navData.timestamp) / 1000000;
  if (USE_TOFS && tofPos.xValid){
    /* if(hms->data.navLogLevel >= 1){ Serial.print("xValid, setting pos X: "); Serial.println(navData.posX); } */
    navData.posX = predNavData.posX + gain[0] * (tofPos.x - predNavData.posX);
    navData.velX = predNavData.velX + gain[1] * (tofPos.x - predNavData.posX) / delT;
    navData.accX = predNavData.accX + gain[2] * (tofPos.x - predNavData.posX) / (0.5 * delT * delT);
  }
  else{
    /* if(hms->data.navLogLevel >= 1){ Serial.println("using predNavData for x"); } */
    navData.posX = predNavData.posX;
    navData.velX = predNavData.velX;
    navData.accX = predNavData.accX;
  }

  if (USE_TOFS && tofPos.yValid){
    /* if(hms->data.navLogLevel >= 1){ Serial.print("yValid, setting pos Y: "); Serial.println(navData.posY); } */
    navData.posY = predNavData.posY + gain[0] * (tofPos.y - predNavData.posY);
    navData.velY = predNavData.velY + gain[1] * (tofPos.y - predNavData.posY) / delT;
    navData.accY = predNavData.accY + gain[2] * (tofPos.y - predNavData.posY) / (0.5 * delT * delT);
  }
  else{
    /* if(hms->data.navLogLevel >= 1){ Serial.println("using predNavData for y"); } */
    navData.posY = predNavData.posY;
    navData.velY = predNavData.velY;
    navData.accY = predNavData.accY;
  }

//  if (USE_IMU && imuEstimate.angXy != FLT_INVALID){
//    navData.angXy = predNavData.angXy + gain[3] * (imuEstimate.angXy - predNavData.angXy);
//    navData.angVelXy = predNavData.angVelXy + gain[4] * (imuEstimate.angXy - predNavData.angXy) / delT;
//    navData.angAccXy = predNavData.angAccXy + gain[5] * (imuEstimate.angXy - predNavData.angXy) / (0.5 * delT * delT);
//  }
//  else if (USE_TOFS && tofPos.yawValid){
  if (USE_TOFS && tofPos.yawValid){
    navData.angXy = predNavData.angXy + gain[3] * (tofPos.yaw - predNavData.angXy);
    navData.angVelXy = predNavData.angVelXy + gain[4] * (tofPos.yaw - predNavData.angXy) / delT;
    navData.angAccXy = predNavData.angAccXy + gain[5] * (tofPos.yaw - predNavData.angXy) / (0.5 * delT * delT);
  }
  else{
    navData.angXy = predNavData.angXy;
    navData.angVelXy = predNavData.angVelXy;
    navData.angAccXy = predNavData.angAccXy;
  }

  navData.timestamp = predNavData.timestamp;
}
