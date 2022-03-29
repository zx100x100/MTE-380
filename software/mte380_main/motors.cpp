#include <analogWrite.h>
#include "motors.h"

#define LEFT_DRIVE_1 17
#define LEFT_DRIVE_2 16
#define RIGHT_DRIVE_1 4
#define RIGHT_DRIVE_2 2

// #define PROP_PIN 27
// #define PROP_MIN 55
// #define PROP_MAX 150

Motors::Motors(Hms* hms):
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
