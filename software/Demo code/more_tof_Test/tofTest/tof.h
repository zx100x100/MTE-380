#ifndef TOF_H
#define TOF_H

#include <Arduino.h>
#include <Wire.h>
#include "vl53lx_class.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


struct TofData {
  uint32_t dist;
  uint32_t numObjs;
  uint32_t count;
  uint32_t timeoutCount;
};


class Tof{
  public:
    Tof();
    Tof(VL53LX* tof_sensor, uint8_t tof_index);
    void poll();
    TofData& getData();
    bool init();
    /* void displayDetails(); */
    bool needsToBeInitialized = true;

  private:
    TofData tofData;
    VL53LX* sensor_vl53lx_sat;
    uint8_t NewDataReady = 0;
    unsigned long lastReading = 0;
    uint8_t index;
    char report[64];
    int status;
    int consecutiveBadReadings = 0;

    bool print = false;
};

#endif
