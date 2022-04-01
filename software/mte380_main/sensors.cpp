#include "sensors.h"
#include "hms_and_cmd_data.pb.h"
#include "math_utils.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?

Sensors::Sensors(Hms* hms, VL53LX *tof_objects, Motors* motors):
  hms(hms),
  motors(motors)
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
void Sensors::initGyro(){
  imu = Imu(hms);
  delay(10);
  imu.poll();
  delay(10);
  fusion.setup(imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);
}

bool Sensors::init(){
  initGyro();

  // " this is the last thing ill try "
  // - ahmad
  //
  // he was talking about eng right?

  delay(5);
  for (int i=0; i<4; i++){
    tof[i] = Tof(hms, sensor_vl53lx_sat[i], i);
    delay(5);
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

float Sensors::getGyroAngle(){
  imu.poll();

  if (imu.getData().accelZ == 0 && imu.getData().accelZ == 0 && imu.getData().accelZ == 0){
    /* imu.getData().accelZ = -10; */
      Serial.println("1 more chance to prove sober imU");
    /* delay(4); */
    /* imu.poll(); */
    /* if (imu.getData().accelZ == 0 && imu.getData().accelZ == 0 && imu.getData().accelZ == 0){ */

      /* motors->setAllToZero(); */
      /* while(true){ */
      /* Serial.println("go home imu u r drunk"); */
      /* [> Serial.println("but we give u one more try"); <] */
        /* delay(1000); */
      /* } */
    /* } */
    hms->greenLedState = LED_ON;
    fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ);
  }
  else{
    fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);
    hms->greenLedState = LED_OFF;
  /* Serial.println("fusion done"); */
  }

  float yaw = -rad2deg(fusion.yaw());
  return yaw;
}
float Sensors::getGyroAnglePitch(){
  imu.poll();


  fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  float pitch = rad2deg(fusion.roll());

  return pitch;
}

uint32_t Sensors::getTofDist(int n){
  uint32_t dist = tof[n].getData().dist;
  /* if (tof[n].needsToBeInitialized){ */
  /* } */
  return dist;
}


void Sensors::update(bool noTofs){
  if (hms->data.sensorsLogLevel >= 2) Serial.println("Sensors::update()");
  imu.poll();
  /* delay(3); */
  if (!noTofs){
    for (int i=0; i<3; i++){ // NOTE IVE DISABLED TOF 3 (back) by changing 4 to 3 in for() !!!!
      tof[i].poll();
      /* delay(3); */
    }
  }
//  if (hms->data.sensorsLogLevel >= 2) Serial.println("finished updating bat voltage");

  timestamp = micros();
}
