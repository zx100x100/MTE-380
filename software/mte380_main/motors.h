#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include <Arduino.h>

#include "hms.h"
#include "guidance_data.pb.h"

class Motors{
  public:
    Motors(GuidanceData& guidanceData,
           Hms* hms);
    void update();

  private:
    GuidanceData& guidanceData;
    Hms* hms;

    void setAllToZero();

};

#endif
