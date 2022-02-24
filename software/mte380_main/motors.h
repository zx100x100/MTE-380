#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include <Arduino.h>

#include "guidance_data.pb.h"

class Motors{
  public:
    Motors();
    Motors(GuidanceData& guidanceData, int leftDrivePin, int rightDrivePin);
    void update();

  private:
    int leftDrivePin;
    int rightDrivePin;
    GuidanceData& guidanceData;
    void setAllToZero();

};

#endif
