#include "sensors.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

Sensors::Sensors(Hms* hms)//:
  /* hms(hms) */
{
  hms = hms;
  imu = Imu(hms);
  for (int i=0; i<4; i++){
    tof[i] = Tof(hms);
  }
  timestamp = 0;
}

// TODO: Zach
void Sensors::updateBatteryVoltage(){
  // analogRead(V_SENSE_PIN);
  float voltage = 1.0;
  hms->data.batteryVoltage = voltage;
  if (hms->data.batteryVoltage < MIN_CELL_VOLTAGE*hms->data.nCells){
    /* hms.logError(HmsData_Error_LOW_BATTERY); */
  }
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
  updateBatteryVoltage();

  timestamp = micros();
}
