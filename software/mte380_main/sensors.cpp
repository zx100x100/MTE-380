#include "sensors.h"
#include "hms_data.pb.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

Sensors::Sensors(Hms* hms):
  hms(hms)
{
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
  HmsData data = hms->hmsData; // HAVE TO ASSIGN THIS OR IT CRASHES... WHY>????? IDKKKKKKK
  data.batteryVoltage = voltage;
  if (data.batteryVoltage < MIN_CELL_VOLTAGE*data.nCells){
    hms->logError(HmsData_Error_LOW_BATTERY, "blah");
  }
}

// SensorData& Sensors::getData(){
  // return sensorData;
// }

void Sensors::update(){
  imu.poll();
  for (int i=0; i<4; i++){
    tof[i].poll();
  }
  updateBatteryVoltage();

  timestamp = micros();
}
