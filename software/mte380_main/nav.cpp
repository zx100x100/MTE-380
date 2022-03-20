#include "fusion.h"
#include "nav.h"

#define L_Y_DELTA 0.328084  // ft
#define L_X_OFFSET 0.164042  // ft
#define F_Y_OFFSET 0.328084  // ft
#define F_X_OFFSET 0  // ft
#define B_Y_OFFSET 0.328084  // ft
#define B_X_OFFSET 0  // ft

#define TRACK_DIM 6  // ft
#define MAX_DEVIATION 0.328084  // ft

#define USE_IMU false
#define USE_TOFS true

#define STARTING_X 3.5
#define STARTING_Y 5.5
#define STARTING_YAW 180

#define FLT_INVALID 0xFFFF

Nav::Nav(Sensors& sensors, CmdData* cmdData, Hms* hms):
  sensors(sensors),
  cmdData(cmdData),
  hms(hms)
{
  navData = NavData_init_zero;
  navData.posX = STARTING_X;
  navData.posY = STARTING_Y;
  navData.angVelXy = STARTING_YAW;

  fusion = Fusion();
  /* fusion.setup(); */

  // TODO use sensorData instead of init_reading
  /* fusion.setup(init_reading.accel_x, init_reading.accel_y, init_reading.accel_z);   */
}

void Nav::init(){
  /* fusion.setup(sensors.imu.getData().accelX, sensors.imu.getData().accelY, sensors.imu.getData().accelZ); */
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
    return sensors.tof[tofNum].getData().dist * 0.00328084;
}

NavData Nav::calculateImu(){
  return NavData_init_zero;
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

TofPosition Nav::calculateTof(){
  TofPosition pos;

  if (tofsUpdated()){
//      if (hms->data.navLogLevel >= 2) Serial.println("tofs updated");
      float estimateLeft, estimateFront, angFromWall;
      if (isValid(L_FRONT) && isValid(L_BACK) && (fmod(navData.angXy, 90) < 35 || fmod(navData.angXy, 90) > 55)) {
//        if (hms->data.navLogLevel >= 2) Serial.println("Left valid");

        angFromWall = rad2deg(atan((getTofFt(L_FRONT) - getTofFt(L_BACK)) / L_Y_DELTA));
        pos.yaw = round(navData.angXy / 90) + angFromWall;

        // The following assumes L_BACK and L_FRONT symmetrical about center of beep boop
        estimateLeft = ((getTofFt(L_FRONT) + getTofFt(L_BACK)) / 2 + L_X_OFFSET) * cosd(angFromWall);
      }
      else{
//        if (hms->data.navLogLevel >= 2) Serial.println("Left invalid");

         // TODO: use gyro?
        pos.yaw = FLT_INVALID;
        estimateLeft = FLT_INVALID;

        // make angFromWall based on previous position
        if (fmod(navData.angXy, 90) < 45){ // angFromWall should be > 0
          angFromWall = fmod(navData.angXy, 90);
        }
        else{  // angFromWall should be < 0
          angFromWall = fmod(navData.angXy, 90) - 90;
        }
      }

      if (isValid(FRONT) && isValid(BACK)){
        if (getTofFt(FRONT) <= getTofFt(BACK))
          estimateFront = (getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall);
        else
          estimateFront = TRACK_DIM - (getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall);
      }
      else if(isValid(FRONT))
        estimateFront = (getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall);
      else if(isValid(BACK))
        estimateFront = TRACK_DIM - (getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall);
      else
        estimateFront = FLT_INVALID;

      switch(int(round(navData.angXy / 90)) % 4){
        case 0:
          pos.x = (estimateFront == FLT_INVALID) ? TRACK_DIM - estimateFront : FLT_INVALID;
          pos.y = estimateLeft;
          break;
        case 1:
          pos.x = (estimateLeft == FLT_INVALID) ? TRACK_DIM - estimateLeft : FLT_INVALID;
          pos.y = (estimateFront == FLT_INVALID) ? TRACK_DIM - estimateFront : FLT_INVALID;
        case 2:
          pos.x = estimateFront;
          pos.y = (estimateLeft == FLT_INVALID) ? TRACK_DIM - estimateLeft : FLT_INVALID;
          break;
        case 3:
          pos.x = estimateLeft;
          pos.y = estimateFront;
      }
  }
  else{
    pos.x = FLT_INVALID; pos.y = FLT_INVALID; pos.yaw = FLT_INVALID;
  }
  return pos;
}

void Nav::update(){
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
  }

  float delT = sensors.timestamp - navData.timestamp;
  NavData pred = getPred(delT);
  if (hms->data.navLogLevel >= 1){
    Serial.print("pred: x: "); Serial.print(pred.posX); Serial.print(", y: ");  Serial.print(pred.posY); Serial.print(", yaw: ");  Serial.println(pred.angXy);
  }

  if (hms->data.navLogLevel >= 1){
    Serial.print("tof: FR: "); Serial.print(getTofFt(FRONT)); Serial.print(", LF: ");  Serial.print(getTofFt(L_FRONT)); Serial.print(", BF: ");  Serial.print(getTofFt(L_BACK)); Serial.print(", BA: ");  Serial.println(getTofFt(BACK));
  }
  TofPosition tofEstimate = calculateTof();
  if (hms->data.navLogLevel >= 1){
    Serial.print("tof estimate: x: "); Serial.print(tofEstimate.x); Serial.print(", y: ");  Serial.print(tofEstimate.y); Serial.print(", yaw: ");  Serial.println(tofEstimate.yaw);
  }
  NavData imuEstimate = calculateImu();

  updateEstimate(imuEstimate, tofEstimate, pred);
  if (hms->data.navLogLevel >= 1){
    Serial.print("estimate: x: "); Serial.print(navData.posX); Serial.print(", y: ");  Serial.print(navData.posY); Serial.print(", yaw: ");  Serial.println(navData.angXy);
  }
}

NavData& Nav::getData(){
  return navData;
}

bool Nav::isValid(TofOrder tof){
//  if (sensors.tof[tof].getData().dist > TRACK_DIM / 2)
//    return false;
//  return true;

  float angFromWall;
  if (fmod(navData.angXy, 90) < 45){ // angFromWall should be > 0
    angFromWall = fmod(navData.angXy, 90);
  }
  else{  // angFromWall should be < 0
    angFromWall = fmod(navData.angXy, 90) - 90;
  }

  float estimateLeft, estimateFront;
  switch(int(round(navData.angXy / 90)) % 4){
    case 0:
      estimateLeft = navData.posY;
      estimateFront = TRACK_DIM - navData.posX;
      break;
    case 1:
      estimateLeft = TRACK_DIM - navData.posX;
      estimateFront = TRACK_DIM - navData.posY;
      break;
    case 2:
      estimateLeft = TRACK_DIM - navData.posY;
      estimateFront = navData.posX;
      break;
    case 3:
      estimateLeft = navData.posX;
      estimateFront = navData.posY;
  }

  switch (tof){
    case FRONT:
      return estimateFront - ((getTofFt(FRONT) + F_Y_OFFSET) * cosd(angFromWall) + F_X_OFFSET * sind(angFromWall)) <= MAX_DEVIATION;
      break;
    case L_FRONT:
      return estimateLeft - ((getTofFt(L_FRONT) + L_X_OFFSET) * cosd(angFromWall) - L_Y_DELTA / 2 * sin(angFromWall)) <= MAX_DEVIATION;
      break;
    case L_BACK:
      return estimateLeft - ((getTofFt(L_BACK) + L_X_OFFSET) * cosd(angFromWall) + L_Y_DELTA / 2 * sind(angFromWall)) <= MAX_DEVIATION;
      break;
    case BACK:
      return estimateFront - (TRACK_DIM - (getTofFt(BACK) + B_Y_OFFSET) * cosd(angFromWall) - B_X_OFFSET * sind(angFromWall)) <= MAX_DEVIATION;
  }
}

NavData Nav::getPred(float delT){
  NavData pred;
  pred.angAccXy = navData.angAccXy;
  pred.angAccXz = navData.angAccXz;
  pred.angAccYz = navData.angAccYz;
  pred.angVelXy = navData.angVelXy + navData.angAccXy * delT;
  pred.angVelXz = navData.angVelXz + navData.angAccXz * delT;
  pred.angVelYz = navData.angVelYz + navData.angAccYz * delT;
  pred.angXy = navData.angXy + navData.angVelXy * delT + navData.angAccXy * delT * delT / 2;
  pred.angXy = navData.angXy + navData.angVelXy * delT + navData.angAccXy * delT * delT / 2;
  pred.angYz = navData.angYz + navData.angVelYz * delT + navData.angAccYz * delT * delT / 2;

  // TODO: take rotation into account
  pred.accX = navData.accX;
  pred.accY = navData.accY;
  pred.accZ = navData.accZ;
  pred.velX = navData.velX + navData.accX * delT;
  pred.velY = navData.velY + navData.accY * delT;
  pred.velZ = navData.velZ + navData.accZ * delT;
  pred.posX = navData.posX + navData.velX * delT + navData.accX * delT * delT / 2;
  pred.posY = navData.posY + navData.velY * delT + navData.accY * delT * delT / 2;
  pred.posZ = navData.posZ + navData.velZ * delT + navData.accZ * delT * delT / 2;

  pred.timestamp = navData.timestamp + delT;
}

void Nav::updateEstimate(const NavData imuEstimate, const TofPosition tofEstimate, const NavData pred){
  if (USE_IMU && imuEstimate.posX != 0){
    // TODO: implement this
  }
  else if (USE_TOFS){
    float delT = pred.timestamp - navData.timestamp;
    if (tofEstimate.x != FLT_INVALID){
      navData.posX = pred.posX + gain[0] * (tofEstimate.x - pred.posX);
      navData.velX = pred.velX + gain[1] * (tofEstimate.x - pred.posX) / delT;
      navData.accX = pred.accX + gain[2] * (tofEstimate.x - pred.posX) / (0.5 * delT * delT);
    }
    else{
      navData.posX = pred.posX;
      navData.velX = pred.velX;
      navData.accX = pred.accX;
    }

    if (tofEstimate.y != FLT_INVALID){
      navData.posY = pred.posY + gain[0] * (tofEstimate.y - pred.posY);
      navData.velX = pred.velY + gain[1] * (tofEstimate.y - pred.posY) / delT;
      navData.accY = pred.accY + gain[2] * (tofEstimate.y - pred.posY) / (0.5 * delT * delT);
    }
    else{
      navData.posY = pred.posY;
      navData.velY = pred.velY;
      navData.accY = pred.accY;
    }

    if (tofEstimate.yaw != FLT_INVALID){
      navData.angXy = pred.angXy + gain[3] * (tofEstimate.yaw - pred.angXy);
      navData.angVelXy = pred.angVelXy + gain[4] * (tofEstimate.yaw - pred.angXy) / delT;
      navData.angAccXy = pred.angAccXy + gain[5] * (tofEstimate.yaw - pred.angXy) / (0.5 * delT * delT);
    }
    else{
      navData.angXy = pred.angXy;
      navData.angVelXy = pred.angVelXy;
      navData.angAccXy = pred.angAccXy;
    }

    navData.timestamp = pred.timestamp;
  }
  else{
    navData = pred;
  }
}
