#include "sensors.h"
#include "hms_and_cmd_data.pb.h"
#include "tof_data.pb.h"
#include "math_utils.h"
#include "FreeRTOSConfig.h"

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
  i2cMutexHandle = xSemaphoreCreateBinary();
  xSemaphoreGive(i2cMutexHandle);
  Serial.printf("handle(init): %p\n", (void*)i2cMutexHandle);
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
  // i2cMutexHandle = xSemaphoreCreateMutex();
  initGyro();

  fusion.setup(imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  // " this is the last thing ill try "
  // - ahmad
  //
  // he was talking about eng right?

  SemaphoreHandle_t newMutexHandle = xSemaphoreCreateMutex();

  for (int i=0; i<4; i++){
    // struct TofInfo newTofInfo = {sensor_vl53lx_sat[i], TofData_init_zero, i, hms, newMutexHandle, i2cMutexHandle};
    tofInfo[i] = TofInfo{sensor_vl53lx_sat[i], TofData_init_zero, i, hms, newMutexHandle, i2cMutexHandle};

    xTaskCreatePinnedToCore(
        tofTask, /* Task function. */
        "Tof Task", /* name of task. */
        2000, /* Stack size of task */
        &tofInfo[i], /* parameter of the task */
        1, /* priority of the task */
        &xHandle[i], // TODO does this need to be xHandle + sizeof(xHandle) * i or something?
        0
    ); /* Task handle to keep track of created task */
    Serial.println("made a task");
    heap_caps_check_integrity(MALLOC_CAP_8BIT, true);
    vTaskDelay(10);
    // tof[i] = Tof(hms, sensor_vl53lx_sat[i], i);
  }
  timestamp = 0;
}



float Sensors::getGyroAngle(){
  Serial.println("getGyroAngle");
  // heap_caps_check_integrity(MALLOC_CAP_8BIT, true);
  Serial.println("heap check complete");
  imu.poll();
  Serial.print("imu.getData().accelZ: "); Serial.println(imu.getData().accelZ);

  if (imu.getData().accelZ == 0 && imu.getData().accelZ == 0 && imu.getData().accelZ == 0){
    while(true){
      Serial.println("go home imu u r drunk");
      delay(1000);
    }
  }

  fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);
  Serial.println("fusion done");

  float yaw = -rad2deg(fusion.yaw());
  return yaw;
}

float Sensors::getGyroAnglePitch(){
  imu.poll();

  fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  float pitch = rad2deg(fusion.pitch());

  return pitch;
}

// float Sensors::getGyroAngleRoll(){
  // imu.poll();

  // fusion.update(imu.getData().gyroX, imu.getData().gyroY, imu.getData().gyroZ, imu.getData().accelX, imu.getData().accelY, imu.getData().accelZ);

  // float roll = rad2deg(fusion.roll());

  // return roll;
// }

void Sensors::initGyro(){
  imu = Imu(hms, i2cMutexHandle);
  imu.poll();
}

uint32_t Sensors::getTofDist(int n){
  xSemaphoreTake(tofInfo[n].dataMutex, MUTEX_TIMEOUT_TICKS);
  uint32_t dist = tofInfo[n].tofData.dist;
  xSemaphoreGive(tofInfo[n].dataMutex);
  return dist;
}

void Sensors::tofWatchdog(){
  for (int i=0; i<4; i++){
    xSemaphoreTake(tofInfo[i].dataMutex, MUTEX_TIMEOUT_TICKS);
    unsigned long timeSinceLastPollMs = (micros() - tofInfo[i].tofData.lastPolled)/1000;
    xSemaphoreGive(tofInfo[i].dataMutex);
    if (timeSinceLastPollMs > TOF_TIMEOUT_MS){
      Serial.println("Watchdog go brrrrrrrr");
      vTaskDelete(xHandle[i]);
      Serial.println("We killed it");

      xTaskCreatePinnedToCore(
        tofTask, /* Task function. */
        "Tof Task", /* name of task. */
        10000, /* Stack size of task */
        &tofInfo[i], /* parameter of the task */
        1, /* priority of the task */
        &xHandle[i],
        0
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
