#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include <Arduino.h>
#include <ESP32Servo.h>

#include "hms.h"
#include "guidance_data.pb.h"

class Motors{
  public:
    Motors(GuidanceData& guidanceData,
           Hms* hms);
    void update();
    void setToShit(float leftPower, float rightPower);

    void setAllToZero();

  private:
    GuidanceData& guidanceData;
    Hms* hms;
    /* Servo propServo; */

};

#endif
