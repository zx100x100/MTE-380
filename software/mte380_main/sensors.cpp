#include "sensors.h"

Sensors::Sensors(){
  imu = Imu();
  TofData tofData[4];
  for (int i=0; i<4; i++){
    tofs[i] = Tof();
    tofData[i] = tofs[i].getData();
  }
  sensorData = SensorData(imu.getData(), tofData);

}

SensorData& Sensors::getData(){
  return sensorData;
}

/* void Sensors::init(){ */
/* } */

void Sensors::update(){
  imu.poll();
  for (int i=0; i<4; i++){
    tofs[i].poll();
  }
  sensorData.timestamp = micros();
}
