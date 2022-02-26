#include <analogWrite.h>

#include "motors.h"

#define LEFT_DRIVE_PIN 4
#define RIGHT_DRIVE_PIN 2

Motors::Motors(GuidanceData& guidanceData,
               Hms* hms):
  guidanceData(guidanceData),
  hms(hms)
{
  setAllToZero();
}

void Motors::setAllToZero(){
  analogWrite(LEFT_DRIVE_PIN, 0);
  analogWrite(RIGHT_DRIVE_PIN, 0);
}

void Motors::update(){
  analogWrite(LEFT_DRIVE_PIN, guidanceData.leftPower);
  analogWrite(RIGHT_DRIVE_PIN, guidanceData.rightPower);
}
