#ifndef TOF_H
#define TOF_H

#include "hms.h"
#include "tof_data.pb.h"

#include <Arduino.h>
#include <Wire.h>
#include "vl53lx_class.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

class Tof{
  public:
    Tof();
    Tof(Hms* hms, uint8_t mux_addr);
    void poll();
    TofData& getData();
    /* void displayDetails(); */

  private:
    TofData tofData;
    Hms* hms;
    uint8_t mux_address = 0;
    VL53LX sensor_vl53lx_sat;

};

#endif
