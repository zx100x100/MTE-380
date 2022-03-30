#ifndef SENSORS_H
#define SENSORS_H

#include <imuFilter.h>
#include "src/VL53L3CX_lib/vl53lx_class.h"

#include "hms.h"
#include "tof.h"
#include "imu.h"


enum TofOrder {FRONT, L_FRONT, L_BACK, BACK};

class Sensors{
  public:
    /*Sensors(); */
    Sensors(Hms* hms, VL53LX tof_objects[]);
    bool init();
    void initGyro();
    float getGyroAngle();
    float getGyroAnglePitch();
    float getGyroAngleRoll();
    // Sensors& getData();
    /* void update(bool skipTOFs=false, bool skipJustBackTof=false); */
    uint32_t getTofDist(int n);
    void tofWatchdog();
    Imu imu;
    long timestamp;
    Hms* hms;
  private:
    SemaphoreHandle_t i2cMutexHandle;
    // TODO update IMU and TOFs
    void updateBatteryVoltage();
    VL53LX* sensor_vl53lx_sat[4];
    TaskHandle_t xHandle[4];
    TofInfo tofInfo[4];
    constexpr static float imuGain = 0.1;
    imuFilter<&imuGain> fusion;
};

#endif
