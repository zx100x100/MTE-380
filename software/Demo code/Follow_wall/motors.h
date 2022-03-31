#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include <Arduino.h>


#define PWM_FREQ 2000


class Motors{
  public:
    Motors();
    void setPower(float leftPower, float rightPower);

    void setAllToZero();

  private:

};

#endif
