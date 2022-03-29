#include "sensors.h"
#include "hms_and_cmd_data.pb.h"
#include "tof_data.pb.h"
#include "math_utils.h"

#define V_SENSE_PIN 15
#define MIN_CELL_VOLTAGE 3 // TODO update value?
#define TOF_TIMEOUT_MS 100

void tofTask(void* tofInfo){
    Tof tof = Tof(static_cast<TofInfo*>(tofInfo));
    tof.init();
    Serial.println("Started");
    int prevCount = -1;
    for(;;){
        tof.poll();
        if (tof.getData().count != prevCount){
            Serial.printf("Count: %d, Dist: %d, numObjs: %d, timeoutCount: %d\n",
            tof.getData().count, tof.getData().dist, tof.getData().numObjs, tof.getData().timeoutCount);
            prevCount = tof.getData().count;
        }
        delay(5);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}

Sensors::Sensors(Hms* hms, VL53LX *tof_objects):
  hms(hms)
{
  for (int i = 0; i < 4; ++i){
    pinMode(tofPins[i], OUTPUT);
    digitalWrite(tofPins[i], LOW);
  }
  for (int i = 0; i < 4; ++i){
    sensor_vl53lx_sat[i] = &tof_objects[i];
    xHandle[i] = NULL;
  }
  updateBatteryVoltage();
}

bool Sensors::init(){
  initGyro();

  fusion.setup(imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  // " this is the last thing ill try "
  // - ahmad
  //
  // he was talking about eng right?

  Wire.begin();
  Wire.setClock(400000);

  for (int i=0; i<4; i++){
    struct TofInfo newTofInfo = {sensor_vl53lx_sat[i], TofData_init_zero, i, hms};
    tofInfo[i] = newTofInfo;
    xTaskCreate(
        tofTask, /* Task function. */
        "Tof Task", /* name of task. */
        10000, /* Stack size of task */
        &tofInfo[i], /* parameter of the task */
        1, /* priority of the task */
        &xHandle[i] // TODO does this need to be xHandle + sizeof(xHandle) * i or something?
    ); /* Task handle to keep track of created task */
    // tof[i] = Tof(hms, sensor_vl53lx_sat[i], i);
  }
  timestamp = 0;
}



float Sensors::getGyroAngle(){
  imu.poll();

  fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  float yaw = -rad2deg(fusion.yaw());
  return yaw;
}

float Sensors::getGyroAnglePitch(){
  imu.poll();

  fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  float pitch = rad2deg(fusion.pitch());

  return pitch;
}

float Sensors::getGyroAngleRoll(){
  imu.poll();

  fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  float roll = rad2deg(fusion.roll());

  return roll;
}

void Sensors::initGyro(){
  imu = Imu(hms);
  imu.poll();
}

uint32_t Sensors::getTofDist(int n){
  return tofInfo[n].tofData.dist;
}

void Sensors::tofWatchdog(){
  for (int i=0; i<4; i++){
    if ((micros() - tofInfo[i].tofData.lastPolled)/1000 > TOF_TIMEOUT_MS){
      Serial.println("Watchdog go brrrrrrrr");
      vTaskDelete(xHandle[i]);
      Serial.println("We killed it");

      xTaskCreate(
        tofTask, /* Task function. */
        "Tof Task", /* name of task. */
        10000, /* Stack size of task */
        &tofInfo[i], /* parameter of the task */
        1, /* priority of the task */
        &xHandle[i]
      ); /* Task handle to keep track of created task */
      Serial.println("made another task");
    }
  }
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

// void Sensors::update(bool skipTOFs, bool skipJustBackTof){
  // if (hms->data.sensorsLogLevel >= 2) Serial.println("Sensors::update()");
  // imu.poll();
  // for (int i=0; i<4; i++){
    // if (skipJustBackTof && i == 3){
      // break;
    // }
    // tof[i].poll();
  // }
  // timestamp = micros();
// }
