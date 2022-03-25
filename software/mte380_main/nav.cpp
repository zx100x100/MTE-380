#include "nav.h"

#define L_Y_DELTA 0.3958  // ft
#define L_X_OFFSET 0.213  // ft
#define F_Y_OFFSET 0.246  // ft
#define F_X_OFFSET 0.156  // ft
#define B_Y_OFFSET 0.246  // ft
#define B_X_OFFSET 0.156  // ft

#define TRACK_DIM 6  // ft
#define MAX_DEVIATION 0.328084  // ft

#define USE_IMU false
#define USE_TOFS true

#define STARTING_X 4.5
#define STARTING_Y 5.5
#define STARTING_YAW 180

Nav::Nav(Sensors& sensors, CmdData* cmdData, Hms* hms):
  sensors(sensors),
  cmdData(cmdData),
  hms(hms)
{
  navData = NavData_init_zero;
  navData.posX = STARTING_X;
  navData.posY = STARTING_Y;
  navData.angXy = STARTING_YAW;
}

void Nav::init(){
  //set up sensor fusion lib
  /* fusion.setup(sensors.imu.getData().accelX, sensors.imu.getData().accelY, sensors.imu.getData().accelZ); */

  unsigned long startT = micros();
  unsigned long curT = 0;
  unsigned long initTime = 1000000;
  while(true){
    getGyroAngle();
    curT = micros();
    if (curT - startT > initTime){
      break;
    }
  }
}

float Nav::rad2deg(float rad){
  return rad * 180 / PI;
}

float Nav::deg2rad(float deg){
  return deg / 180 * PI;
}

float Nav::cosd(float deg){
  return cos(deg2rad(deg));
}

float Nav::sind(float deg){
  return sin(deg2rad(deg));
}

float Nav::getTofFt(TofOrder tofNum){
    return sensors.tof[tofNum].getData().dist * 0.00328084; //mm -> ft
}

NavData Nav::calculateImu(){
  return NavData_init_zero;
}

float Nav::getGyroAngle(){
  /* sensors.imu.poll(); */

  /* fusion.update(sensors.imu.getData().gyroX, sensors.imu.getData().gyroY, sensors.imu.getData().gyroZ, sensors.imu.getData().accelX, sensors.imu.getData().accelY, sensors.imu.getData().accelZ); */

  /* float yaw = rad2deg(fusion.yaw()); */
  float yaw = 5;
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

void Nav::updateTof(GuidanceData_Heading heading){
  if (tofsUpdated()){
      float angFromWall, front, left;
      bool angFromWallValid, frontValid, leftValid;

      if (hms->data.navLogLevel >= 2) Serial.println("tofs updated");
      if (isValid(L_FRONT) && isValid(L_BACK)) {//TODO && (fmod(navData.angXy, 90) < 35 || fmod(navData.angXy, 90) > 55)) {
        angFromWallValid = true;
        leftValid = true;
        if (hms->data.navLogLevel >= 2) Serial.println("Left valid");

        angFromWall = rad2deg(atan((getTofFt(L_FRONT) - getTofFt(L_BACK)) / L_Y_DELTA));
        // The following assumes L_BACK and L_FRONT symmetrical about center of beep boop
        left = ((getTofFt(L_FRONT) + getTofFt(L_BACK)) / 2 + L_X_OFFSET) * cosd(angFromWall);
      }
      else{
        if (hms->data.navLogLevel >= 2) Serial.println("Left invalid");
        angFromWallValid = false;
        leftValid = false;
      }

      if (isValid(FRONT) && isValid(BACK)){ // If both valid, use the shortest
        frontValid = true;
        if (getTofFt(FRONT) <= getTofFt(BACK))
          front = (getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall);
        else
          front = TRACK_DIM - (getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall);
      }
      else if(isValid(FRONT)){ // If not, use front if valid
        frontValid = true;
        front = (getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall);
      }
      else if(isValid(BACK)){ // If not, use back if valid
        frontValid = true;
        front = TRACK_DIM - (getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall);
      }
      else{ // Well Fuck
        if (hms->data.navLogLevel >= 2) Serial.println("vertical tofs INVALID");
        frontValid = false;
      }

    if(hms->data.navLogLevel >= 1){ Serial.print("Heading: "); Serial.println(heading); }
    tofPos.yawValid = angFromWallValid;  // TODO: validate angle change
    
    // Local to global coordinate conversion switch case:
    tofPos.left = left;
    tofPos.front = front;
    tofPos.angFromWall = angFromWall;
    switch(heading){
      case GuidanceData_Heading_UP:
        tofPos.xValid = leftValid && fabs(tofPos.x - left) < MAX_DEVIATION;
        tofPos.yValid = frontValid && fabs(tofPos.y - front) < MAX_DEVIATION;
        if (tofPos.xValid) tofPos.x = left;
        if (tofPos.yValid) tofPos.y = front;
        if (angFromWallValid) tofPos.yaw = angFromWall + 270;
        break;
      case GuidanceData_Heading_RIGHT:
        tofPos.xValid = frontValid && fabs(tofPos.x - (TRACK_DIM - front)) < MAX_DEVIATION;
        tofPos.yValid = leftValid && fabs(tofPos.y - left) < MAX_DEVIATION;
        if (tofPos.xValid) tofPos.x = TRACK_DIM - front;
        if (tofPos.yValid) tofPos.y = left;
        if (angFromWallValid) tofPos.yaw = angFromWall;
        break;
      case GuidanceData_Heading_DOWN:
        tofPos.xValid = leftValid && fabs(tofPos.x - (TRACK_DIM - left)) < MAX_DEVIATION;
        tofPos.yValid = frontValid && fabs(tofPos.y - (TRACK_DIM - front)) < MAX_DEVIATION;
        if (tofPos.xValid) tofPos.x = TRACK_DIM - left;
        if (tofPos.yValid) tofPos.y = TRACK_DIM - front;
        if (angFromWallValid) tofPos.yaw = angFromWall + 90;
        break;
      case GuidanceData_Heading_LEFT:
        tofPos.xValid = frontValid && fabs(tofPos.x - front) < MAX_DEVIATION;
        tofPos.yValid = leftValid && fabs(tofPos.y - (TRACK_DIM - left)) < MAX_DEVIATION;
        if (tofPos.xValid) tofPos.x = front;
        if (tofPos.yValid) tofPos.y = TRACK_DIM - left;
        if (angFromWallValid) tofPos.yaw = angFromWall + 180;
        break;
    }
  }
  else{ // ToFs not updated
    if (hms->data.navLogLevel >= 2) Serial.println("tofs NOT updated");
    tofPos.xValid = false;
    tofPos.yValid = false;
    tofPos.yawValid = false;
  }
}

void Nav::update(GuidanceData_Heading heading){
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

  float delT = float(sensors.timestamp - navData.timestamp) / 1000000; // navData timestamp is the same as previous sensor timestamp
  NavData pred = getPred(delT);
  if (hms->data.navLogLevel >= 1){
    Serial.print("pred: x: "); Serial.print(pred.posX); Serial.print(", y: ");  Serial.print(pred.posY); Serial.print(", yaw: ");  Serial.print(pred.angXy); Serial.print(", dt: "); Serial.println(delT);
  }

  updateTof(heading);
  if (hms->data.navLogLevel >= 1){
    Serial.print("tof: FR: "); Serial.print(getTofFt(FRONT)); Serial.print(", LF: ");  Serial.print(getTofFt(L_FRONT)); Serial.print(", LB: ");  Serial.print(getTofFt(L_BACK)); Serial.print(", BA: ");  Serial.println(getTofFt(BACK));
    Serial.print("tof estimate: x: "); Serial.print(tofPos.x); Serial.print(", left: ");  Serial.print(tofPos.y); Serial.print(", yaw: ");  Serial.println(tofPos.yaw);
  }

  updateEstimate(pred);
  if (hms->data.navLogLevel >= 1){
    Serial.print("estimate: x: "); Serial.print(navData.posX); Serial.print(", y: ");  Serial.print(navData.posY); Serial.print(", yaw: ");  Serial.println(navData.angXy);
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

NavData Nav::getPred(float delT){  // delT in seconds
  NavData pred;
  pred.angAccXy = navData.angAccXy;
  pred.angAccXz = navData.angAccXz;
  pred.angAccYz = navData.angAccYz;
  pred.angVelXy = navData.angVelXy;
  pred.angVelXz = navData.angVelXz;
  pred.angVelYz = navData.angVelYz;
  pred.angXy = navData.angXy;
  pred.angXz = navData.angXz;
  pred.angYz = navData.angYz;  // TODO: do we just want it to equal the previous one?

  // TODO: take rotation into account
  pred.accX = navData.accX;
  pred.accY = navData.accY;
  pred.accZ = navData.accZ;
  pred.velX = navData.velX + navData.accX * delT;
  pred.velY = navData.velY + navData.accY * delT;
  pred.velZ = navData.velZ + navData.accZ * delT;
  pred.posX = navData.posX + navData.velX * delT; // + navData.accX * delT * delT / 2;
  pred.posY = navData.posY + navData.velY * delT; // + navData.accY * delT * delT / 2;
  pred.posZ = navData.posZ + navData.velZ * delT; // + navData.accZ * delT * delT / 2;

  pred.timestamp = navData.timestamp + delT * 1000000;

  return pred;
}

void Nav::updateEstimate(const NavData pred){
  float delT = float(pred.timestamp - navData.timestamp) / 1000000;
  if (USE_TOFS && tofPos.xValid){
    if(hms->data.navLogLevel >= 1){ Serial.print("xValid, setting pos X: "); Serial.println(navData.posX); }
    navData.posX = pred.posX + gain[0] * (tofPos.x - pred.posX);
    navData.velX = pred.velX + gain[1] * (tofPos.x - pred.posX) / delT;
    navData.accX = pred.accX + gain[2] * (tofPos.x - pred.posX) / (0.5 * delT * delT);
  }
  else{
    if(hms->data.navLogLevel >= 1){ Serial.println("using pred for x"); }
    navData.posX = pred.posX;
    navData.velX = pred.velX;
    navData.accX = pred.accX;
  }

  if (USE_TOFS && tofPos.yValid){
    if(hms->data.navLogLevel >= 1){ Serial.print("yValid, setting pos Y: "); Serial.println(navData.posY); }
    navData.posY = pred.posY + gain[0] * (tofPos.y - pred.posY);
    navData.velX = pred.velY + gain[1] * (tofPos.y - pred.posY) / delT;
    navData.accY = pred.accY + gain[2] * (tofPos.y - pred.posY) / (0.5 * delT * delT);
  }
  else{
    if(hms->data.navLogLevel >= 1){ Serial.println("using pred for y"); }
    navData.posY = pred.posY;
    navData.velY = pred.velY;
    navData.accY = pred.accY;
  }

//  if (USE_IMU && imuEstimate.angXy != FLT_INVALID){
//    navData.angXy = pred.angXy + gain[3] * (imuEstimate.angXy - pred.angXy);
//    navData.angVelXy = pred.angVelXy + gain[4] * (imuEstimate.angXy - pred.angXy) / delT;
//    navData.angAccXy = pred.angAccXy + gain[5] * (imuEstimate.angXy - pred.angXy) / (0.5 * delT * delT);
//  }
//  else if (USE_TOFS && tofPos.yawValid){
  if (USE_TOFS && tofPos.yawValid){
    navData.angXy = pred.angXy + gain[3] * (tofPos.yaw - pred.angXy);
    navData.angVelXy = pred.angVelXy + gain[4] * (tofPos.yaw - pred.angXy) / delT;
    navData.angAccXy = pred.angAccXy + gain[5] * (tofPos.yaw - pred.angXy) / (0.5 * delT * delT);
  }
  else{
    navData.angXy = pred.angXy;
    navData.angVelXy = pred.angVelXy;
    navData.angAccXy = pred.angAccXy;
  }

  navData.timestamp = pred.timestamp;
}
