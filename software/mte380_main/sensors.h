#ifndef SENSORS_H
#define SENSORS_H

#include "tof.h"
#include "imu.h"

class Sensors{
  public:
    Sensors();
    // void init();
    // Sensors& getData();
    void update();
    Imu imu;
    Tof tof[4];
    long timestamp;
  private:
    // SensorData sensorData;
};

#endif
