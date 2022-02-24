#include <analogWrite.h>

#include "motors.h"

Motors::Motors(GuidanceData& guidanceData, int leftDrivePin, int rightDrivePin)
  :guidanceData(guidanceData)
  ,leftDrivePin(leftDrivePin)
  ,rightDrivePin(rightDrivePin)
{
  setAllToZero();
}

void Motors::setAllToZero(){
  analogWrite(leftDrivePin, 0);
  analogWrite(rightDrivePin, 0);
}

void Motors::update(){
  analogWrite(leftDrivePin, guidanceData.leftPower);
  analogWrite(rightDrivePin, guidanceData.rightPower);
}
