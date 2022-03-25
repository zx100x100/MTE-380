#include "sensors.h"
#include "hms_and_cmd_data.pb.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

Sensors::Sensors(Hms* hms, VL53LX *tof_objects):
  hms(hms)
{
  for (int i = 0; i < 4; ++i){
    pinMode(tofPins[i], OUTPUT);
    digitalWrite(tofPins[i], LOW);
  }
  for (int i = 0; i < 4; ++i){
    sensor_vl53lx_sat[i] = &tof_objects[i];
  }
  updateBatteryVoltage();
}

bool Sensors::init(){
  // imu = Imu(hms);
  /* imu.poll(); */

  // " this is the last thing ill try "
  // - ahmad
  //
  // he was talking about eng right?

  Wire.begin();
  Wire.setClock(400000);

  for (int i=0; i<4; i++){
    tof[i] = Tof(hms, sensor_vl53lx_sat[i], i);
  }
  timestamp = 0;
}

void Sensors::updateBatteryVoltage(){
  pinMode(V_SENSE_PIN, INPUT);
  float voltage = analogRead(V_SENSE_PIN)*0.0063977;
  hms->data.batteryVoltage = voltage;
  hms->data.nCells = hms->data.batteryVoltage / 3.5;
  // if (hms->data.batteryVoltage < MIN_CELL_VOLTAGE*hms->data.nCells){
    /* hms->logError(HmsData_Error_LOW_BATTERY, "charge ur shit"); */
  // }
}

// SensorData& Sensors::getData(){
  // return sensorData;
// }

void Sensors::update(){
  if (hms->data.sensorsLogLevel >= 2) Serial.println("Sensors::update()");
  /* imu.poll(); */
  for (int i=0; i<4; i++){
    tof[i].poll();
  }
//  if (hms->data.sensorsLogLevel >= 2) Serial.println("finished updating bat voltage");

  timestamp = micros();
}
