#ifndef SENSORS_H
#define SENSORS_H

#include "tof.h"
#include "imu.h"
#include "sensor_data.h"

class Sensors{
  public:
    Sensors();
    // void init();
    SensorData& getData();
    void update();
  private:
    Imu imu;
    Tof tofs[4];
    SensorData sensorData;
};

#endif
