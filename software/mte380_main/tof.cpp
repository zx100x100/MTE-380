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

  Serial.println("begin");

  // Configure VL53LX satellite component.
  sensor_vl53lx_sat->begin();

  Serial.println("init sensor");
  //Initialize VL53LX satellite component.
  sensor_vl53lx_sat->InitSensor(0x12);


  Serial.println("start measurement");
  // Start Measurements
  sensor_vl53lx_sat->VL53LX_StartMeasurement();
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
    if(NewDataReady == 1){
        if (hms->data.sensorsLogLevel >= 2) Serial.println("ready");

        status = sensor_vl53lx_sat->VL53LX_GetMultiRangingData(pMultiRangingData);
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

            status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
            NewDataReady = 0;
            status = sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);  // TODO: what if status bad
        }
    }
    else if (micros() - lastReading > TIMEOUT){
        if (hms->data.sensorsLogLevel >= 1) Serial.println("Timeout");
        status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
        NewDataReady = 0;
        status = sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);  // TODO: what if status bad
    }


//    do
//    {
//        status = sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);
//    } while (!NewDataReady);
//    if (hms->data.sensorsLogLevel >= 2) Serial.println("after");
//
//    if ((!status) && (NewDataReady != 0))
//    {
//        status = sensor_vl53lx_sat->VL53LX_GetMultiRangingData(pMultiRangingData);
////        Serial.print("Status: ");
////        Serial.print(status);
//        no_of_object_found = pMultiRangingData->NumberOfObjectsFound;
//        snprintf(report, sizeof(report), " VL53LX Satellite: Count=%d, #Objs=%1d ", pMultiRangingData->StreamCount, no_of_object_found);
//        if (hms->data.sensorsLogLevel >= 2) Serial.print(report);
//
//        tofData.numObjs = no_of_object_found;
//
//        for (j = 0; j < no_of_object_found && j < MAX_OBJS; j++)
//        {
//
//            tofData.dist = pMultiRangingData->RangeData[j].RangeMilliMeter;
//
//            //if(pMultiRangingData->RangeData[j].RangeStatus == 0){
//            if (hms->data.sensorsLogLevel >= 2) {
//                Serial.print("status=");
//                Serial.print(pMultiRangingData->RangeData[j].RangeStatus);
//                Serial.print(", D=");
//                Serial.print(pMultiRangingData->RangeData[j].RangeMilliMeter);
//                Serial.print("mm");
//                Serial.print(", Signal=");
//                Serial.print((float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps / 65536.0);
//                Serial.print(" Mcps, Ambient=");
//                Serial.print((float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps / 65536.0);
//                Serial.print(" Mcps");
//            }
//        }
//        if (status == 0)
//        {
//            status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement();
//        }
//        NewDataReady = 0;
//    }
}
