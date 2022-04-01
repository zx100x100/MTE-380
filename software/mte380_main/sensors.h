#ifndef SENSORS_H
#define SENSORS_H

#include "hms.h"
#include "tof.h"
#include "imu.h"
#include "motors.h"

#include <imuFilter.h>

#include "src/VL53L3CX_lib/vl53lx_class.h"

enum TofOrder {FRONT, L_FRONT, L_BACK, BACK};


class Sensors{
  public:
    /*Sensors(); */
    Sensors(Hms* hms, VL53LX tof_objects[], Motors* motors);
    bool init();
    void initGyro();
    // Sensors& getData();
    void update(bool noTofs=false);
    Imu imu;
    Tof tof[4];
    long timestamp;
    Hms* hms;
    float getGyroAngle();
    float getGyroAnglePitch();
    uint32_t getTofDist(int n);
  private:
    // TODO update IMU and TOFs
    void updateBatteryVoltage();
    VL53LX* sensor_vl53lx_sat[4];
    constexpr static float imuGain = 0.1;
    imuFilter<&imuGain> fusion;

    float lastBackSensorVal;
    Motors* motors;
};

#endif
