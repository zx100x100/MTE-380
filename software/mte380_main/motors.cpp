#include <analogWrite.h>
#include "motors.h"

#define LEFT_DRIVE_PIN 4
#define RIGHT_DRIVE_PIN 2

#define PROP_PIN 27

#define PROP_MIN 55
#define PROP_MAX 150

Motors::Motors(GuidanceData& guidanceData,
               Hms* hms):
  guidanceData(guidanceData),
  hms(hms)
{
  setAllToZero();
  propServo.attach(PROP_PIN);
}

void Motors::setAllToZero(){
  analogWrite(LEFT_DRIVE_PIN, 0);
  analogWrite(RIGHT_DRIVE_PIN, 0);
  propServo.write(PROP_MIN);
}

void Motors::update(){
  analogWrite(LEFT_DRIVE_PIN, guidanceData.leftPower);
  analogWrite(RIGHT_DRIVE_PIN, guidanceData.rightPower);
  float propPower = map(guidanceData.propPower, 0, 255, PROP_MIN, PROP_MAX);
  if (hms->data.logLevel >= 1){
    Serial.print("propPower: "); Serial.println(propPower);
  }
  propServo.write(propPower);
}
