#include "sensors.h"
#include "hms_and_cmd_data.pb.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

#define TCAADDR 0x70

#define MUX_S1 27
#define MUX_S2 14

Sensors::Sensors(Hms* hms, VL53LX *tof_objects):
  hms(hms)
{
  for (int i = 0; i < 4; ++i){
  sensor_vl53lx_sat[i] = &tof_objects[i];
  }

  mux_addresses[FRONT] = 2;
  mux_addresses[L_FRONT] = 1;
  mux_addresses[L_BACK] = 0;
  mux_addresses[BACK] = 3;
}

bool Sensors::init(){
  /* return true; */
  imu = Imu(hms);

  Serial.println("WIRE begin");
  Wire.begin();
  Wire.setClock(1000000);

  for (int i=0; i<4; i++){
    Serial.println("Starting mux shit");
    digitalWrite(MUX_S1, mux_addresses[i]&0x01);
    digitalWrite(MUX_S2, mux_addresses[i]&0x02);
    delay(100);
    tof[i] = Tof(hms, sensor_vl53lx_sat[i]);
  }
  timestamp = 0;
}

// TODO: Zach
void Sensors::updateBatteryVoltage(){
  // analogRead(V_SENSE_PIN);
  float voltage = 1.0;
  hms->data.batteryVoltage = voltage;
  if (hms->data.batteryVoltage < MIN_CELL_VOLTAGE*hms->data.nCells){
    /* hms->logError(HmsData_Error_LOW_BATTERY, "charge ur shit"); */
  }
}

// SensorData& Sensors::getData(){
  // return sensorData;
// }

void Sensors::update(){
  /* return; */
  if (hms->data.sensorsLogLevel >= 2) Serial.println("Sensors::update()");
  /* imu.poll(); */
  for (int i=0; i<4; i++){
    digitalWrite(MUX_S1, mux_addresses[i]&0x01);
    digitalWrite(MUX_S2, mux_addresses[i]&0x02);
    tof[i].poll();
  }
  updateBatteryVoltage();
  if (hms->data.sensorsLogLevel >= 2) Serial.println("finished updating bat voltage");

  timestamp = micros();
}
