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
  analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
  analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
  analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
  analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
  // propServo.write(PROP_MIN);
}

void Motors::setPower(float leftPower, float rightPower, bool breakingBool){
  if(hms->data.mainLogLevel >= 2){ Serial.print("leftPower: "); Serial.println(leftPower); }
  if(hms->data.mainLogLevel >= 2){ Serial.print("rightPower: "); Serial.println(rightPower); }
  if(breakingBool == true){
    analogWrite(LEFT_DRIVE_1, fabs(leftPower), PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, fabs(leftPower), PWM_FREQ);
  }
  else if (leftPower >= 0){
    analogWrite(LEFT_DRIVE_1, leftPower, PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
  }
  else{
    analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, -leftPower, PWM_FREQ);
  }
  if (breakingBool == true){
    analogWrite(RIGHT_DRIVE_1, fabs(leftPower), PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, fabs(leftPower), PWM_FREQ);
  }
  else if (rightPower >= 0){
    analogWrite(RIGHT_DRIVE_1, rightPower, PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
  }
  else{
    analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, -rightPower, PWM_FREQ);
  }
}

void Motors::update(){
  if (hms->data.mainLogLevel >= 2) Serial.println("motors");
  if(hms->data.mainLogLevel >= 2){ Serial.print("leftPower: "); Serial.println(guidanceData.leftPower); }
  if(hms->data.mainLogLevel >= 2){ Serial.print("rightPower: "); Serial.println(guidanceData.rightPower); }
  if (guidanceData.leftPower >= 0){
    // Serial.println("MOTORS::::::::::::::;");
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("guidanceData.leftPower: "); Serial.println(guidanceData.leftPower); }
    analogWrite(LEFT_DRIVE_1, guidanceData.leftPower, PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
  }
  else{
    analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, -guidanceData.leftPower, PWM_FREQ);
  }
  if (guidanceData.rightPower >= 0){
    analogWrite(RIGHT_DRIVE_1, guidanceData.rightPower, PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
  }
  else{
    analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, -guidanceData.rightPower, PWM_FREQ);
  }
  // float propPower = map(guidanceData.propPower, 0, 255, PROP_MIN, PROP_MAX);
  // if (hms->data.mainLogLevel >= 2){ Serial.print("propPower: "); Serial.println(propPower);}
  // propServo.write(propPower);
}
