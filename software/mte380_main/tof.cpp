#include "tof.h"

#define TIMEOUT 50000

/* Tof::Tof(){ */
/* } */
Tof::Tof()
{
}

Tof::Tof(Hms* hms, VL53LX* tof_sensor):
  hms(hms),
  sensor_vl53lx_sat(tof_sensor)
{
  tofData = TofData_init_zero;

  if (hms->data.sensorsLogLevel >= 1) Serial.println("begin");

  // Configure VL53LX satellite component.
  // This will set TOF_PLACEHOLDER_PIN as output
  sensor_vl53lx_sat->begin();

  if (hms->data.sensorsLogLevel >= 1) Serial.println("init sensor");
  //Initialize VL53LX satellite component.
  // This will turn TOF_PLACEHOLDER_PIN LOW then HIGH, then continue initializing the sensor
  sensor_vl53lx_sat->InitSensor(0x12);


  if (hms->data.sensorsLogLevel >= 1) Serial.println("start measurement");
  // Start Measurements
  sensor_vl53lx_sat->VL53LX_StartMeasurement();
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
    VL53LX_MultiRangingData_t MultiRangingData;
    VL53LX_MultiRangingData_t* pMultiRangingData = &MultiRangingData;
    unsigned long beforeT = micros();

    sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);  // TODO: what if status bad
    if(NewDataReady == 1){
        NewDataReady = 0;
//        if (hms->data.sensorsLogLevel >= 2) Serial.println("ready");

        status = sensor_vl53lx_sat->VL53LX_GetMultiRangingData(pMultiRangingData);
        unsigned long afterReadingT = micros();
        if (status == 0)
        {
            lastReading = micros();
            tofData.numObjs = pMultiRangingData->NumberOfObjectsFound;
            tofData.count = pMultiRangingData->StreamCount;

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

            unsigned long beforeClearT = micros();
            status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
            unsigned long afterClearT = micros();
//            if (hms->data.sensorsLogLevel >= 2){
//              unsigned long dt = micros() - beforeT;
//              unsigned long dt2 = micros() - afterReadingT;
//              unsigned long dt3 = afterClearT - beforeClearT;
//              Serial.print("Did the first thing in "); Serial.println(dt2);
//              Serial.print("cleared interrupt in "); Serial.println(dt3);
//              Serial.print("Did stuff. elapsed: "); Serial.println(dt);
//            }
        }
    }
    else{
      unsigned long dt = micros() - beforeT;
      /* Serial.print("No data ready. elapsed: "); Serial.println(dt); */
      if (micros() - lastReading > TIMEOUT){
          if (hms->data.sensorsLogLevel >= 1) Serial.println("Timeout");
          status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
          NewDataReady = 0;
          tofData.timeoutCount++;
      }
    }
}
