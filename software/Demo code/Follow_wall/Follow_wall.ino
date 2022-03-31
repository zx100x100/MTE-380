#include "tof.h"
#include "motors.h"
#include <stdlib.h>

#define L_Y_DELTA 0.3958  // ft
#define L_X_OFFSET 0.213  // ft
#define F_Y_OFFSET 0.246  // ft
#define F_X_OFFSET 0.156  // ft
#define B_Y_OFFSET 0.246  // ft
#define B_X_OFFSET 0.156  // ft

#define LEFT_WALL_DIST 2 // ft
#define DRIFT_LOOK_AHEAD_DIST 3 //ft
#define MOTOR_POWER 225
#define MAX_MOTOR_POWER 255

#define RUN_TIMEOUT 5000 //ms
#define START_DELAY 4000 //ms

#define KP 1 //1.1
#define KI 0//0.02
#define KD 0 //2500

#define MAX_DRIFT_ERROR_I 400 //??????????????????

#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // I'm lazy

Motors motors = Motors();

VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, tofPins[0]),
  VL53LX(&Wire, tofPins[1]),
  VL53LX(&Wire, tofPins[2]),
  VL53LX(&Wire, tofPins[3])
};

Tof tof[4];

float angFromWall = 0, left = 0, errDrift = 0, errDriftI = 0;
unsigned long t_prev = 0, deltaT = 100; //set non-zero deltaT

void setup() {
  Serial.begin(115200);
  Serial.println("Gonna follow a wall!");
  // Init TOFs
  Wire.begin();
  Wire.setClock(1000000);
  
  for (int i = 0; i < 4; ++i){
    pinMode(tofPins[i], OUTPUT);
    digitalWrite(tofPins[i], LOW);
    tof[i] = Tof(&sensor_vl53lx_sat[i], i);
  }

  while(millis() < START_DELAY);

  t_prev = micros();
  delay(10); // avoid 0 deltaT

}

void loop() {
  deltaT = micros() - t_prev;
  t_prev += deltaT;

  for (int i=0; i<4; i++){
    tof[i].poll();
    /*if(getTofFt(i) < 0.3 && getTofFt(i) > 0.2){
      motors.setPower(0, 0);
      Serial.println("GOT TOO CLOSE");
      while(true); // stop everything
    }*/
  }
  
  //float angFromWall = rad2deg(atan((getTofFt(1) - getTofFt(2)) / L_Y_DELTA));
  //Serial.print("\n######################### Angle: ");
  //Serial.println(angFromWall);
  //left = ((getTofFt(1) + getTofFt(2)) / 2 + L_X_OFFSET) * cosd(angFromWall);
  //return isValid(1) && isValid(2);

  // DRIFT PID --------------------------------------------------------------------------
  float lastErrDrift = errDrift;
  float dist = left - LEFT_WALL_DIST;

  float desiredAngle = -rad2deg(atan(dist/DRIFT_LOOK_AHEAD_DIST));

  if (!updateWallAngleAndDistance()){
    motors.setPower(MOTOR_POWER, MOTOR_POWER);
    Serial.println("Invalid TOF");
  }
  Serial.print("\n\nFront: ");
  Serial.print(getTofFt(1));
  Serial.print("   Back: ");
  Serial.println(getTofFt(2));

  errDrift = angFromWall - desiredAngle;
  Serial.print("Error drift: ");
  Serial.println(errDrift);
  float errDriftD = (errDrift - lastErrDrift)*1000/deltaT;
  errDriftI += errDrift * deltaT/1000;
  if (sign(lastErrDrift) != sign(errDrift)){
    errDriftI = 0;
  }
  errDriftI = constrainVal(errDriftI, MAX_DRIFT_ERROR_I);

  float P = errDrift * KP;
  float I = errDriftI * KI;
  float D = errDriftD * KD;

  float driftOutput = P + I + D;
  float rightOutputDrift = driftOutput;
  float leftOutputDrift = -driftOutput;
    
  motors.setPower(constrainVal(MOTOR_POWER+leftOutputDrift, MAX_MOTOR_POWER), constrainVal(MOTOR_POWER+rightOutputDrift, MAX_MOTOR_POWER));
  Serial.print("Left: ");
  Serial.print(MOTOR_POWER+leftOutputDrift);
  Serial.print("   Right: ");
  Serial.println(MOTOR_POWER+rightOutputDrift);
  
  delay(25); //frigin EMI

  if(millis() > RUN_TIMEOUT + START_DELAY){
    motors.setPower(0, 0);
    Serial.println("RAN TO MAX TIME");
    while(true); // stop everything
  }

}

// RANDOM FUNCS!!!!!!!!!!

float getTofFt(uint8_t tofNum){
    return tof[tofNum].getData().dist * 0.00328084; //mm -> ft
}

float rad2deg(float rad){
  return rad * 180 / PI;
}

float deg2rad(float deg){
  return deg / 180 * PI;
}

bool updateWallAngleAndDistance(){
  angFromWall = rad2deg(atan((getTofFt(1) - getTofFt(2)) / L_Y_DELTA));
  left = ((getTofFt(1) + getTofFt(2)) / 2 + L_X_OFFSET) * cosd(angFromWall);
  return getTofFt(1) < 6 && getTofFt(1) > 0.25 && getTofFt(2) < 6 && getTofFt(2) > 0.25;
}

float constrainVal(float val, float maximum){
  if (val > 0){
    if (val > maximum){
      return maximum;
    }
    return val;
  }
  if (val < -maximum){
    return -maximum;
  }
  return val;
}

float cosd(float deg){
  return cos(deg2rad(deg));
}
