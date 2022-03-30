#ifndef TOF_H
#define TOF_H

#include "hms.h"
#include "tof_data.pb.h"

#include <Arduino.h>
#include <Wire.h>
#include "src/VL53L3CX_lib/vl53lx_class.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MUTEX_TIMEOUT_TICKS portMAX_DELAY

const static int tofPins[4] = {26, 25, 18, 19};
struct TofInfo {
  VL53LX* sensor;
  TofData tofData;
  uint8_t tofIndex;
  Hms* hms;
  SemaphoreHandle_t dataMutex;
  SemaphoreHandle_t i2cMutexHandle;
};

class Tof{
  public:
    Tof();
    Tof(TofInfo* tofInfo);
    void poll();
    TofData& getData();
    bool init();
    /* void displayDetails(); */
    bool needsToBeInitialized = true;

  private:
    TickType_t mutexTimeout = MUTEX_TIMEOUT_TICKS;
    TofData tofData;
    Hms* hms;
    VL53LX* sensor_vl53lx_sat;
    SemaphoreHandle_t dataMutex;
    SemaphoreHandle_t i2cMutexHandle;
    uint8_t NewDataReady = 0;
    unsigned long lastReading = 0;
    uint8_t index;
    char report[64];
    int status;
    int consecutiveBadReadings = 0;
    unsigned long lastPolled = 0;
};

#endif
