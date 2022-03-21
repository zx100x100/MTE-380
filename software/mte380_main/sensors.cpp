#include "sensors.h"
#include "hms_and_cmd_data.pb.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

#define MUX_S1 27
#define MUX_S2 14

#define TOF_SHUTDOWN_PIN 18  // To be used to power cycle all the TOFs

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
  updateBatteryVoltage();
}

bool Sensors::init(){
  imu = Imu(hms);

  Wire.begin();
  Wire.setClock(400000);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);

  // power cycle the TOFs to initialize them
  pinMode(TOF_SHUTDOWN_PIN, OUTPUT);
  digitalWrite(TOF_SHUTDOWN_PIN, LOW);
  delay(10);
  digitalWrite(TOF_SHUTDOWN_PIN, HIGH);
  delay(10);

  delay(3000);

  for (int i=0; i<4; i++){
    //    Serial.println("Starting mux shit");
    digitalWrite(MUX_S1, mux_addresses[i]&0x01);
    digitalWrite(MUX_S2, (mux_addresses[i]&0x02)>>1);
    delay(100);
    tof[i] = Tof(hms, sensor_vl53lx_sat[i]);
  }
  timestamp = 0;
}

void Sensors::updateBatteryVoltage(){
  pinMode(V_SENSE_PIN, INPUT);
  float voltage = analogRead(V_SENSE_PIN)*0.0063977;
  hms->data.batteryVoltage = voltage;
  // if (hms->data.batteryVoltage < MIN_CELL_VOLTAGE*hms->data.nCells){
    /* hms->logError(HmsData_Error_LOW_BATTERY, "charge ur shit"); */
  // }
}

// SensorData& Sensors::getData(){
  // return sensorData;
// }

void Sensors::update(){
  if (hms->data.sensorsLogLevel >= 2) Serial.println("Sensors::update()");
  imu.poll();
  for (int i=0; i<4; i++){
    digitalWrite(MUX_S1, mux_addresses[i]&0x01);
    digitalWrite(MUX_S2, (mux_addresses[i]&0x02)>>1);
    tof[i].poll();
  }
//  if (hms->data.sensorsLogLevel >= 2) Serial.println("finished updating bat voltage");

  timestamp = micros();
}
