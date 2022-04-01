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
  lastLeftPower = 0;
  lastRightPower = 0;
  // propServo.attach(PROP_PIN);
}

void Motors::setAllToZero(){
  analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
  analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
  analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
  analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
  // propServo.write(PROP_MIN);
}

void Motors::setPower(float leftPower, float rightPower, bool brake){
  if (brake){
    analogWrite(LEFT_DRIVE_1, -fabs(leftPower), PWM_FREQ);
    analogWrite(LEFT_DRIVE_2, -fabs(leftPower), PWM_FREQ);
    analogWrite(RIGHT_DRIVE_1, -fabs(rightPower), PWM_FREQ);
    analogWrite(RIGHT_DRIVE_2, -fabs(rightPower), PWM_FREQ);
    return;
  }

  //lastLeftPower = 100;
  //leftPower = 0;

  float leftMotorChange = leftPower - lastLeftPower; // -100
  float rightMotorChange = rightPower - lastRightPower;
  bool changingDirectionsTooFastLeft = fabs(leftMotorChange) > 50;
  bool changingDirectionsTooFastRight = fabs(rightMotorChange) > 50;
  bool changingDirectionsTooFast = changingDirectionsTooFastLeft || changingDirectionsTooFastRight;

  if (changingDirectionsTooFast){
    leftPower = 
  }
  /* if (leftPower) */
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
  lastLeftPower = leftPower;
  lastRightPower = rightPower;

}
