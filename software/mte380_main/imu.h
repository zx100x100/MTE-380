#ifndef IMU_H
#define IMU_H

#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>

#include "hms.h"
#include "imu_data.pb.h"

#define MUTEX_TIMEOUT_TICKS portMAX_DELAY

class Imu{
  public:
    Imu();
    Imu(Hms* hms, SemaphoreHandle_t i2cMutexHandle);
    // bool init();
    void poll();
    ImuData &getData();
    void displayDetails();
  private:
    TickType_t mutexTimeout = MUTEX_TIMEOUT_TICKS;
    ImuData imuData;
    /* Assign a unique ID to this sensor at the same time */
    Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(01);
    /* Assign a unique ID to this sensor at the same time */
    Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(02);
    /* Assign a unique ID to this sensor at the same time */
    Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(03);
    SemaphoreHandle_t i2cMutexHandle;
    Hms* hms;
};

#endif
