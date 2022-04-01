#include <analogWrite.h>
#include "motors.h"

#define LEFT_DRIVE_1 17
#define LEFT_DRIVE_2 16
#define RIGHT_DRIVE_1 4
#define RIGHT_DRIVE_2 2

#define N_SLEW_SECTIONS 10

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
  bool changingDirectionsTooFastLeft = fabs(leftMotorChange) > 150/N_SLEW_SECTIONS;
  bool changingDirectionsTooFastRight = fabs(rightMotorChange) > 150/N_SLEW_SECTIONS;
  bool changingDirectionsTooFast = changingDirectionsTooFastLeft || changingDirectionsTooFastRight;

  float leftPowerAdjBy = leftMotorChange/N_SLEW_SECTIONS; // -25
  float rightPowerAdjBy = rightMotorChange/N_SLEW_SECTIONS;

  if (changingDirectionsTooFast){
    for(int i=N_SLEW_SECTIONS -1; i>=0; --i){
      float currLeftPower = leftPower - i*leftPowerAdjBy;
      float currRightPower = rightPower - i*rightPowerAdjBy;
      if (abs(currLeftPower) > 170 || abs(currRightPower) > 170){
        hms->redLedState = LED_ON;
        setAllToZero();
        while(true){
          Serial.println("aaaaaaaa why u go backward??!");
          Serial.printf("leftMotor: %5.3f rightMotor: %5.3f leftChange: %5.3f, rightChange: %5.3f currL: %5.3f, currR: %5.3f\n",leftPower, rightPower, leftPowerAdjBy, rightPowerAdjBy, currLeftPower, currRightPower);
          hms->update();
          delay(1000);
        }
      }
      if (currLeftPower >= 0){
        analogWrite(LEFT_DRIVE_1, currLeftPower, PWM_FREQ);
        analogWrite(LEFT_DRIVE_2, 0, PWM_FREQ);
      }
      else{
        analogWrite(LEFT_DRIVE_1, 0, PWM_FREQ);
        analogWrite(LEFT_DRIVE_2, -currLeftPower, PWM_FREQ);
      }
      if (currRightPower >= 0){
        analogWrite(RIGHT_DRIVE_1, currRightPower, PWM_FREQ);
        analogWrite(RIGHT_DRIVE_2, 0, PWM_FREQ);
      }
      else{
        analogWrite(RIGHT_DRIVE_1, 0, PWM_FREQ);
        analogWrite(RIGHT_DRIVE_2, -currRightPower, PWM_FREQ);
      }
      delay(20/N_SLEW_SECTIONS);
    }
  }
  else{
    /* if (leftPower) */
    if (abs(leftPower) > 170 || abs(rightPower > 170)){
      hms->redLedState = LED_ON;
      setAllToZero();
      while(true){
        hms->update();
        Serial.println("aaaaaaaa why u go backward??!");
        delay(1000);
      }
    }
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
  lastLeftPower = leftPower;
  lastRightPower = rightPower;
}
