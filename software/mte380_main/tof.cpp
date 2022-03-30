#include "tof.h"

#define TIMEOUT 50000
#define MAX_TIMEOUTS 5
#define MAX_BAD_READINGS 5

/* Tof::Tof(){ */
/* } */
Tof::Tof(){
}

Tof::Tof(TofInfo* tofInfo){
  sensor_vl53lx_sat = tofInfo->sensor;
  tofData = tofInfo->tofData;
  tofData.dist = 0;
  tofData.numObjs = 0;
  tofData.count = 0;
  tofData.timeoutCount = 0;
  tofData.lastPolled = 0;
  index = tofInfo->tofIndex;
  dataMutex = tofInfo->dataMutex;
  i2cMutexHandle = tofInfo->i2cMutexHandle;
  Serial.printf("handle(tof init): %p\n", (void*)i2cMutexHandle);
  mutexTimeout = MUTEX_TIMEOUT_TICKS;
  init();
}


bool Tof::init(){
  bool initializedProperly = true;
  // if (hms->data.sensorsLogLevel >= 1){ Serial.print("tof"); Serial.print(index); Serial.println("begin"); }

  // Configure VL53LX satellite component.

  Serial.print("semaphore count for take1: ");
  Serial.println(uxSemaphoreGetCount(i2cMutexHandle));
  xSemaphoreTake(i2cMutexHandle, mutexTimeout);
  Serial.println("take1");
  sensor_vl53lx_sat->begin();
  xSemaphoreGive(i2cMutexHandle);
  Serial.println("give1");

  Serial.print("tof"); Serial.print(index);
  //Initialize VL53LX satellite component.
  
  xSemaphoreTake(i2cMutexHandle, mutexTimeout);
  Serial.println("take2");
  initializedProperly &= sensor_vl53lx_sat->InitSensor(0x10 + index*2) == 0;  // ensure sensor initialized properly
  xSemaphoreGive(i2cMutexHandle);
  Serial.println("give2");


  // if (hms->data.sensorsLogLevel >= 1){ Serial.print("tof"); Serial.print(index);  Serial.println("start measurement"); }
  // Start Measurements
  xSemaphoreTake(i2cMutexHandle, mutexTimeout);
  Serial.println("take3");
  sensor_vl53lx_sat->VL53LX_StartMeasurement();

  xSemaphoreGive(i2cMutexHandle);
  Serial.println("give3");
  needsToBeInitialized = false;

  return initializedProperly;
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
  xSemaphoreTake(dataMutex, mutexTimeout);
  Serial.println("take4");
  lastPolled = micros();
  xSemaphoreGive(dataMutex);
  Serial.println("give4");
  VL53LX_MultiRangingData_t MultiRangingData;
  VL53LX_MultiRangingData_t* pMultiRangingData = &MultiRangingData;

  xSemaphoreTake(i2cMutexHandle, mutexTimeout);
  Serial.println("take5");
  status = sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);  // TODO: what if status bad
  xSemaphoreGive(i2cMutexHandle);
  Serial.println("give5");
  if(NewDataReady == 1){
    NewDataReady = 0;  // reset for next time
//    if (hms->data.sensorsLogLevel >= 2) Serial.println("ready");

    if (status == 0)
    {
      xSemaphoreTake(i2cMutexHandle, mutexTimeout);
      Serial.println("take6");
      status = sensor_vl53lx_sat->VL53LX_GetMultiRangingData(pMultiRangingData);
      xSemaphoreGive(i2cMutexHandle);
      Serial.println("give6");
      if (status == 0){
        tofData.numObjs = pMultiRangingData->NumberOfObjectsFound;

        snprintf(report, sizeof(report), " TOF %d: Count=%d, #Objs=%1d ", index, tofData.count, tofData.numObjs);
        // if (hms->data.sensorsLogLevel >= 2) Serial.print(report);
        lastReading = micros();
        if (tofData.numObjs){ // if at least 1 object found
          // if(hms->data.sensorsLogLevel >= 1){ Serial.print("tof"); Serial.print(index); Serial.print("tofData.dist != pMultiRangingData->RangeData[0].RangeMilliMeter: "); Serial.println(tofData.dist != pMultiRangingData->RangeData[0].RangeMilliMeter); }
          // if(hms->data.sensorsLogLevel >= 1){ Serial.print("tof"); Serial.print(index); Serial.print("pMultiRangingData->RangeData[0].RangeMilliMeter: "); Serial.println(pMultiRangingData->RangeData[0].RangeMilliMeter); }
          // if(hms->data.sensorsLogLevel >= 1){ Serial.print("tof"); Serial.print(index); Serial.print("tofData.dist: "); Serial.println(tofData.dist); }
          if (pMultiRangingData->RangeData[0].RangeStatus == 0 && tofData.dist != pMultiRangingData->RangeData[0].RangeMilliMeter){
            consecutiveBadReadings = 0;
            xSemaphoreTake(dataMutex, mutexTimeout);
            tofData.dist = pMultiRangingData->RangeData[0].RangeMilliMeter;
            tofData.count = pMultiRangingData->StreamCount;
            xSemaphoreGive(dataMutex);
          } else{
            consecutiveBadReadings++;
            if (consecutiveBadReadings % MAX_BAD_READINGS == 0){
              needsToBeInitialized = true;
              Serial.print("tof"); Serial.print(index); Serial.println("REBOOTING TOF due to bad readings");
              init();
              return;
            }
          } // TODO: if no objects found, we don't increment consecutiveBadReadings

          if (hms->data.sensorsLogLevel >= 2) {
            Serial.print("tof"); Serial.print(index); 
            Serial.print("status=");
            Serial.print(pMultiRangingData->RangeData[0].RangeStatus);
            Serial.print(", D=");
            Serial.print(pMultiRangingData->RangeData[0].RangeMilliMeter);
            Serial.print("mm");
            Serial.print(", Signal=");
            Serial.print((float)pMultiRangingData->RangeData[0].SignalRateRtnMegaCps / 65536.0);
            Serial.print(" Mcps, Ambient=");
            Serial.print((float)pMultiRangingData->RangeData[0].AmbientRateRtnMegaCps / 65536.0);
            Serial.print(" Mcps \n");
          }
        }
        else{
          Serial.print("tof"); Serial.print(index); Serial.println(" 0 detected");
          consecutiveBadReadings++;
          if (consecutiveBadReadings % MAX_BAD_READINGS == 0){
            needsToBeInitialized = true;
            Serial.print("tof"); Serial.print(index); Serial.println("uniquexxx REBOOTING TOF due to bad readings");
            init();
            return;
          }
        }

        xSemaphoreTake(i2cMutexHandle, mutexTimeout);
        Serial.println("take25");
        status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
        xSemaphoreGive(i2cMutexHandle);
        Serial.println("give25");
      }
      else{
        Serial.print("tof"); Serial.print(index); Serial.println("unique_fuck2");
      }
    }
    else{
      Serial.print("tof"); Serial.print(index); Serial.println("unique_fuck1");
    }
  }
  //else{
    /* Serial.print("No data ready. elapsed: "); Serial.println(dt); */
    if (micros() - lastReading > TIMEOUT){
    if (hms->data.sensorsLogLevel >= 1) { Serial.print("tof"); Serial.print(index); Serial.println("Timeout");}

    xSemaphoreTake(i2cMutexHandle, mutexTimeout);
    Serial.println("take69");
    status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
    xSemaphoreGive(i2cMutexHandle);
    Serial.println("give69");
    NewDataReady = 0;
    xSemaphoreTake(dataMutex, mutexTimeout);
    tofData.timeoutCount++;
    xSemaphoreGive(dataMutex);


    if (tofData.timeoutCount % MAX_TIMEOUTS == 0){
      needsToBeInitialized = true;
      Serial.print("tof"); Serial.print(index); Serial.println("REBOOTING TOF due to timeouts");
      init();
      return;
    }
  }
}
