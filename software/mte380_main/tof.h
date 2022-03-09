#ifndef TOF_H
#define TOF_H

// IF SOMETHING IS WEIRD. CHECK HERE
#define PLACEHOLDER_PIN 100 // DO NOT TRUST THIS

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
    Tof(Hms* hms, VL53LX* tof_sensor);
    void poll();
    TofData& getData();
//    bool init();
    /* void displayDetails(); */

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
