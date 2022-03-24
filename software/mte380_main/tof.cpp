#include "tof.h"

#define TIMEOUT 50000

/* Tof::Tof(){ */
/* } */
Tof::Tof()
{
}

bool Tof::init(uint8_t tof_addr){

  bool initializedProperly = true;
  if (hms->data.sensorsLogLevel >= 1) Serial.println("begin");

  // Configure VL53LX satellite component.
  // This will set TOF_PLACEHOLDER_PIN as output
  sensor_vl53lx_sat->begin();

  if (hms->data.sensorsLogLevel >= 1) Serial.println("init tof sensor");
  //Initialize VL53LX satellite component.
  // This will turn TOF_PLACEHOLDER_PIN LOW then HIGH, then continue initializing the sensor
  initializedProperly &= sensor_vl53lx_sat->InitSensor(0x10 + tof_addr) == 0;  // ensure sensor initialized properly


  if (hms->data.sensorsLogLevel >= 1) Serial.println("start measurement");
  // Start Measurements
  sensor_vl53lx_sat->VL53LX_StartMeasurement();

  needsToBeInitialized = false;

  return initializedProperly;
}

Tof::Tof(Hms* hms, VL53LX* tof_sensor, uint8_t tof_addr):
  hms(hms),
  sensor_vl53lx_sat(tof_sensor)
{
  tofData = TofData_init_zero;
  init(tof_addr);
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
    VL53LX_MultiRangingData_t MultiRangingData;
    VL53LX_MultiRangingData_t* pMultiRangingData = &MultiRangingData;

    status = sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);  // TODO: what if status bad
    if(NewDataReady == 1){
        NewDataReady = 0;  // reset for next time
//        if (hms->data.sensorsLogLevel >= 2) Serial.println("ready");

        if (status == 0)
        {
            status = sensor_vl53lx_sat->VL53LX_GetMultiRangingData(pMultiRangingData);
            if (status == 0){
                tofData.numObjs = pMultiRangingData->NumberOfObjectsFound;
                tofData.count = pMultiRangingData->StreamCount;
                if(tofData.count != 0) lastReading = micros();

                snprintf(report, sizeof(report), " VL53LX Satellite: Count=%d, #Objs=%1d ", tofData.count, tofData.numObjs);
                if (hms->data.sensorsLogLevel >= 2) Serial.print(report);

                if (tofData.numObjs){ // if at least 1 object found
                    tofData.dist = pMultiRangingData->RangeData[0].RangeMilliMeter;

                    if (hms->data.sensorsLogLevel >= 2) {
                        Serial.print("status=");
                        Serial.print(pMultiRangingData->RangeData[0].RangeStatus);
                        Serial.print(", D=");
                        Serial.print(pMultiRangingData->RangeData[0].RangeMilliMeter);
                        Serial.print("mm");
                        Serial.print(", Signal=");
                        Serial.print((float)pMultiRangingData->RangeData[0].SignalRateRtnMegaCps / 65536.0);
                        Serial.print(" Mcps, Ambient=");
                        Serial.print((float)pMultiRangingData->RangeData[0].AmbientRateRtnMegaCps / 65536.0);
                        Serial.print(" Mcps");
                    }
                }

                status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
            }
        }
    }
    //else{
      /* Serial.print("No data ready. elapsed: "); Serial.println(dt); */
      if (micros() - lastReading > TIMEOUT){
        if (hms->data.sensorsLogLevel >= 1) Serial.println("Timeout");
        status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
        NewDataReady = 0;
        tofData.timeoutCount++;

        if (tofData.timeoutCount % 50 == 0){
          needsToBeInitialized = true;
        }
      }
    //}
}
