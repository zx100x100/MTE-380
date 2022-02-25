#ifndef IMU_H
#define IMU_H

#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>

#include "hms.h"
#include "imu_data.pb.h"

class Imu{
  public:
    Imu();
    Imu(Hms* hms);
    // bool init();
    void poll();
    ImuData &getData();
    void displayDetails();
  private:
    ImuData imuData;
    /* Assign a unique ID to this sensor at the same time */
    Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(01);
    /* Assign a unique ID to this sensor at the same time */
    Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(02);
    /* Assign a unique ID to this sensor at the same time */
    Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(03);

    Hms* hms;
};

#endif
