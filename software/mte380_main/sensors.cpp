#include "sensors.h"
#include "hms_and_cmd_data.pb.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

#define FRONT_TOF 4
#define REAR_TOF 5
#define L_BACK_TOF 3
#define L_FRONT_TOF 2
uint8_t mux_addresses[4] = { FRONT_TOF, REAR_TOF, L_BACK_TOF, L_FRONT_TOF };

#define TCAADDR 0x70

Sensors::Sensors(Hms* hms):
  hms(hms)
{
  imu = Imu(hms);

  Wire.begin();

//
//  for (int i=0; i<4; i++){
//    Wire.beginTransmission(TCAADDR);
//    Wire.write(1 << mux_addresses[i]);
//    Wire.endTransmission();
//    delay(100);
//    tof[i] = Tof(hms);
//  }
  timestamp = 0;
}

// TODO: Zach
void Sensors::updateBatteryVoltage(){
  // analogRead(V_SENSE_PIN);
  float voltage = 1.0;
  HmsData data = hms->data; // HAVE TO ASSIGN THIS OR IT CRASHES... WHY>????? IDKKKKKKK
  data.batteryVoltage = voltage;
  if (data.batteryVoltage < MIN_CELL_VOLTAGE*data.nCells){
    hms->logError(HmsData_Error_LOW_BATTERY, "charge ur shit");
  }
}

// SensorData& Sensors::getData(){
  // return sensorData;
// }

void Sensors::update(){
  if (hms->data.sensorsLogLevel >= 2) Serial.println("Sensors::update()");
  /* imu.poll(); */
//  for (int i=0; i<4; i++){
//    Wire.beginTransmission(TCAADDR);
//    Wire.write(1 << mux_addresses[i]);
//    Wire.endTransmission();
//    tof[i].poll();
//  }
  updateBatteryVoltage();
  if (hms->data.sensorsLogLevel >= 2) Serial.println("finished updating bat voltage");

  timestamp = micros();
}
