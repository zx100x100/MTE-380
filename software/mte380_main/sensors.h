#ifndef SENSORS_H
#define SENSORS_H

#include "hms.h"
#include "tof.h"
#include "imu.h"

class Sensors{
  public:
    /* Sensors(); */
    Sensors(Hms* hms);
    // void init();
    // Sensors& getData();
    void update();
    Imu imu;
    Tof tof[4];
    long timestamp;
    Hms* hms;
  private:
    // TODO update IMU and TOFs
    void updateBatteryVoltage();
};

#endif
