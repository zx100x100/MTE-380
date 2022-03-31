#include <analogWrite.h>
#include "motors.h"

#define LEFT_DRIVE_1 17
#define LEFT_DRIVE_2 16
#define RIGHT_DRIVE_1 4
#define RIGHT_DRIVE_2 2

Motors::Motors(){
  setAllToZero();
}

void Motors::setAllToZero(){
  analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
  analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
  analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
  analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
}

void Motors::setPower(float leftPower, float rightPower){
  if (leftPower >= 0){
    analogWrite(LEFT_DRIVE_1, leftPower, PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
  }
  else{
    analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, -leftPower, PWM_FREQ);
  }
  if (rightPower >= 0){
    analogWrite(RIGHT_DRIVE_1, rightPower, PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
  }
  else{
    analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, -rightPower, PWM_FREQ);
  }
}
