#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include <Arduino.h>
//#include <ESP32Servo.h>

#include "hms.h"

#define PWM_FREQ 2000

class Motors{
  public:
    Motors(Hms* hms);
    void update();
    void setPower(float leftPower, float rightPower, bool breakingBool = false);

    void setAllToZero();

  private:
    Hms* hms;
    /* Servo propServo; */

};

#endif
