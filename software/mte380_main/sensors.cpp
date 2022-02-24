#include "sensors.h"

Sensors::Sensors(){
  imu = Imu();
  for (int i=0; i<4; i++){
    tof[i] = Tof();
  }
  timestamp = 0;
}

// SensorData& Sensors::getData(){
  // return sensorData;
// }

/* void Sensors::init(){ */
/* } */
void Sensors::update(){
  imu.poll();
  for (int i=0; i<4; i++){
    tof[i].poll();
  }
  timestamp = micros();
}
