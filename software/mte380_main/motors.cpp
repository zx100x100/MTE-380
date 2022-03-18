#include <analogWrite.h>
#include "motors.h"

#define LEFT_DRIVE_1 16
#define LEFT_DRIVE_2 17
#define RIGHT_DRIVE_1 2
#define RIGHT_DRIVE_2 4

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

void Motors::update(){
  if (hms->data.mainLogLevel >= 2) Serial.println("motors");
  if (guidanceData.leftPower >= 0){
    if (guidanceData.leftPower >= 1){
      analogWrite(LEFT_DRIVE_1, 255);
      analogWrite(LEFT_DRIVE_2, 0);
    }
    /* analogWrite(LEFT_DRIVE_1, guidanceData.leftPower); */
    /* analogWrite(LEFT_DRIVE_2, 0); */
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
