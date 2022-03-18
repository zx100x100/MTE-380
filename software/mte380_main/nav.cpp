#include "fusion.h"
#include "nav.h"

#define L_Y_DELTA 100  // mm
#define L_X_OFFSET 50  // mm
#define F_Y_OFFSET 100  // mm
#define F_X_OFFSET 0  // mm
#define B_Y_OFFSET 100  // mm
#define B_X_OFFSET 0  // mm

#define TRACK_DIM 1828.8  // mm
#define MAX_DEVIATION 50  // mm

#define USE_IMU false
#define USE_TOFS true

#define STARTING_X 1066.8
#define STARTING_Y 1676.4
#define STARTING_YAW 180

#define FLT_INVALID 0xFFFF

Nav::Nav(Sensors& sensors, Hms* hms):
  sensors(sensors),
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

NavData Nav::calculateImu(){
  return NavData_init_zero;
}

bool Nav::tofsUpdated(){  // TODO: Check this
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
      if (hms->data.navLogLevel >= 2) Serial.println("tofs updated");
      float estimateLeft, estimateFront;
      if (isValid(L_FRONT) && isValid(L_BACK) && (int(navData.angXy) % 90 < 40 || int(navData.angXy) > 50)) {
        if (hms->data.navLogLevel >= 2) Serial.println("Left valid");

        float theta = rad2deg(atan((sensors.tof[L_FRONT].getData().dist - sensors.tof[L_BACK].getData().dist) / L_Y_DELTA));
        pos.yaw = round(navData.angXy / 90) + theta;

        // The following assumes L_BACK and L_FRONT symmetrical about center of beep boop
        estimateLeft = ((sensors.tof[L_FRONT].getData().dist + sensors.tof[L_BACK].getData().dist) / 2 + L_X_OFFSET) * cos(deg2rad(pos.yaw));
      }
      else{ // TODO: use gyro?
        if (hms->data.navLogLevel >= 2) Serial.println("Left invalid");
        pos.yaw = FLT_INVALID;
        estimateLeft = FLT_INVALID;
      }

      if (isValid(FRONT) && isValid(BACK)){ // TODO: do we wanna use pos.yaw or navData.angXy
        if (sensors.tof[FRONT].getData().dist <= sensors.tof[BACK].getData().dist)
          estimateFront = (sensors.tof[FRONT].getData().dist + F_Y_OFFSET) * abs(cos(deg2rad(pos.yaw))) + F_X_OFFSET * sin(deg2rad(pos.yaw));
        else
          estimateFront = TRACK_DIM - (sensors.tof[BACK].getData().dist + B_Y_OFFSET) * abs(cos(deg2rad(pos.yaw))) - B_X_OFFSET * sin(deg2rad(pos.yaw));
      }
      else if(isValid(FRONT))
        estimateFront = (sensors.tof[FRONT].getData().dist + F_Y_OFFSET) * abs(cos(deg2rad(pos.yaw))) + F_X_OFFSET * sin(deg2rad(pos.yaw));
      else if(isValid(BACK))
        estimateFront = TRACK_DIM - (sensors.tof[BACK].getData().dist + B_Y_OFFSET) * abs(cos(deg2rad(pos.yaw))) - B_X_OFFSET * sin(deg2rad(pos.yaw));
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
  if (hms->data.navLogLevel >= 2) Serial.println("Nav::update()");

  float delT = sensors.timestamp - navData.timestamp;
  NavData pred = getPred(delT);
  if (hms->data.navLogLevel >= 1){
    Serial.print("pred: x: "); Serial.print(pred.posX); Serial.print(", y: ");  Serial.print(pred.posY); Serial.print(", yaw: ");  Serial.println(pred.angXy);
  }

  if (hms->data.navLogLevel >= 1){
    Serial.print("tof: FR: "); Serial.print(sensors.tof[0].getData().dist); Serial.print(", LF: ");  Serial.print(sensors.tof[1].getData().dist); Serial.print(", BF: ");  Serial.print(sensors.tof[2].getData().dist); Serial.print(", BA: ");  Serial.println(sensors.tof[3].getData().dist);
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
  if (sensors.tof[tof].getData().dist > TRACK_DIM / 2)
    return false;
  return true;

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
    case FRONT: // TODO: check absolute values of sin + cos
      return estimateFront - ((sensors.tof[FRONT].getData().dist + F_Y_OFFSET) * abs(cos(deg2rad(navData.angXy))) + F_X_OFFSET * abs(sin(deg2rad(navData.angXy)))) <= MAX_DEVIATION;
      break;
    case L_FRONT:
      if (navData.angXy / 90 - int(navData.angXy / 90) <= 0.5){
        return estimateLeft - ((sensors.tof[L_FRONT].getData().dist + L_X_OFFSET) * cos(deg2rad(navData.angXy)) + L_Y_DELTA / 2 * abs(sin(deg2rad(navData.angXy)))) <= MAX_DEVIATION;
      }
      else{
        return estimateLeft - ((sensors.tof[L_FRONT].getData().dist + L_X_OFFSET) * cos(deg2rad(navData.angXy)) - L_Y_DELTA / 2 * abs(sin(deg2rad(navData.angXy)))) <= MAX_DEVIATION;
      }
      break;
    case L_BACK:
      if (navData.angXy / 90 - int(navData.angXy / 90) <= 0.5){
        return estimateLeft - ((sensors.tof[L_BACK].getData().dist + L_X_OFFSET) * cos(deg2rad(navData.angXy)) - L_Y_DELTA / 2 * abs(sin(deg2rad(navData.angXy)))) <= MAX_DEVIATION;
      }
      else{
        return estimateLeft - ((sensors.tof[L_BACK].getData().dist + L_X_OFFSET) * cos(deg2rad(navData.angXy)) + L_Y_DELTA / 2 * abs(sin(deg2rad(navData.angXy)))) <= MAX_DEVIATION;
      }
      break;
    case BACK:
      return estimateFront - (TRACK_DIM - (sensors.tof[BACK].getData().dist + B_Y_OFFSET) * cos(deg2rad(navData.angXy)) - B_X_OFFSET * sin(deg2rad(navData.angXy))) <= MAX_DEVIATION;
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
  if (USE_IMU && imuEstimate.posX != 0){  // TODO: make sure this never fucks up
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
