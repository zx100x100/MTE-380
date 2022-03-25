#include "tof.h"

#define TIMEOUT 50000

/* Tof::Tof(){ */
/* } */
Tof::Tof()
{
}

bool Tof::init(){

  bool initializedProperly = true;
  if (hms->data.sensorsLogLevel >= 1) Serial.println("begin");

  // Configure VL53LX satellite component.
  sensor_vl53lx_sat->begin();

  if (hms->data.sensorsLogLevel >= 1) Serial.println("init tof sensor");
  //the dumb library doesn't understand that if you reboot the device, it goes back to the default address, so we need to remind it.
  sensor_vl53lx_sat->VL53LX_SetDeviceAddress(VL53LX_DEFAULT_DEVICE_ADDRESS);
  //Initialize VL53LX satellite component.
  initializedProperly &= sensor_vl53lx_sat->InitSensor(0x10 + index*2) == 0;  // ensure sensor initialized properly


  if (hms->data.sensorsLogLevel >= 1) Serial.println("start measurement");
  // Start Measurements
  sensor_vl53lx_sat->VL53LX_StartMeasurement();

  needsToBeInitialized = false;

  return initializedProperly;
}

Tof::Tof(Hms* hms, VL53LX* tof_sensor, uint8_t tof_index):
  hms(hms),
  sensor_vl53lx_sat(tof_sensor)
{
  tofData = TofData_init_zero;
  index = tof_index;
  init();
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

                snprintf(report, sizeof(report), " TOF %d: Count=%d, #Objs=%1d ", index, tofData.count, tofData.numObjs);
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
                        Serial.print(" Mcps \n");
                    }
                }

                status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
            }
            else{
              Serial.println("unique_fuck2");
            }
        }
        else{
          Serial.println("unique_fuck1");
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
