#ifndef TOF_H
#define TOF_H

const static int tofPins[4] = {26, 25, 18, 19};

#include "hms.h"
#include "tof_data.pb.h"

#include <Arduino.h>
#include <Wire.h>
#include "vl53lx_class.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


class Tof{
  public:
    Tof();
    Tof(Hms* hms, VL53LX* tof_sensor, uint8_t tof_addr);
    void poll();
    TofData& getData();
    bool init(uint8_t tof_addr);
    /* void displayDetails(); */
    bool needsToBeInitialized = true;

  private:
    TofData tofData;
    Hms* hms;
    VL53LX* sensor_vl53lx_sat;
    uint8_t NewDataReady = 0;
    unsigned long lastReading = 0;
    char report[64];
    int status;

};

#endif
