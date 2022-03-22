#include <analogWrite.h>
#include "motors.h"

#define LEFT_DRIVE_1 17
#define LEFT_DRIVE_2 16
#define RIGHT_DRIVE_1 4
#define RIGHT_DRIVE_2 2

// #define PROP_PIN 27

// #define PROP_MIN 55
// #define PROP_MAX 150

Motors::Motors(GuidanceData& guidanceData,
               Hms* hms):
  guidanceData(guidanceData),
  hms(hms)
{
  setAllToZero();
  // propServo.attach(PROP_PIN);
}

void Motors::setAllToZero(){
  analogWrite(LEFT_DRIVE_1, 0);
  analogWrite(LEFT_DRIVE_2, 0);
  analogWrite(RIGHT_DRIVE_1, 0);
  analogWrite(RIGHT_DRIVE_2, 0);
  // propServo.write(PROP_MIN);
}

void Motors::setToShit(float leftPower, float rightPower){
  if (leftPower >= 0){
    analogWrite(LEFT_DRIVE_1, leftPower);
    analogWrite(LEFT_DRIVE_2, 0);
  }
  else{
    analogWrite(LEFT_DRIVE_1, 0);
    analogWrite(LEFT_DRIVE_2, -leftPower);
  }
  if (rightPower >= 0){
    analogWrite(RIGHT_DRIVE_1, rightPower);
    analogWrite(RIGHT_DRIVE_2, 0);
  }
  else{
    analogWrite(RIGHT_DRIVE_1, 0);
    analogWrite(RIGHT_DRIVE_2, -rightPower);
  }
}

void Motors::update(){
  if (hms->data.mainLogLevel >= 2) Serial.println("motors");
  if (guidanceData.leftPower >= 0){
    // Serial.println("MOTORS::::::::::::::;");
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("guidanceData.leftPower: "); Serial.println(guidanceData.leftPower); }
    analogWrite(LEFT_DRIVE_1, guidanceData.leftPower);
    analogWrite(LEFT_DRIVE_2, 0);
  }
  else{
    analogWrite(LEFT_DRIVE_1, 0);
    analogWrite(LEFT_DRIVE_2, -guidanceData.leftPower);
  }
  if (guidanceData.rightPower >= 0){
    analogWrite(RIGHT_DRIVE_1, guidanceData.rightPower);
    analogWrite(RIGHT_DRIVE_2, 0);
  }
  else{
    analogWrite(RIGHT_DRIVE_1, 0);
    analogWrite(RIGHT_DRIVE_2, -guidanceData.rightPower);
  }
  // float propPower = map(guidanceData.propPower, 0, 255, PROP_MIN, PROP_MAX);
  // if (hms->data.mainLogLevel >= 2){ Serial.print("propPower: "); Serial.println(propPower);}
  // propServo.write(propPower);
}
