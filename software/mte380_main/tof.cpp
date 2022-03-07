#include "tof.h"

// IF SOMETHING IS WEIRD. CHECK HERE
#define PLACEHOLDER_PIN 0 // DO NOT TRUST THIS

#define TCAADDR 0x70


/* Tof::Tof(){ */
/* } */
Tof::Tof():
  sensor_vl53lx_sat(&Wire, PLACEHOLDER_PIN)
{  
}

Tof::Tof(Hms* hms):
  hms(hms),
  sensor_vl53lx_sat(&Wire, PLACEHOLDER_PIN)
{
  tofData = TofData_init_zero;

  // Configure VL53LX satellite component.
  sensor_vl53lx_sat.begin();


  //Initialize VL53LX satellite component.
  sensor_vl53lx_sat.InitSensor(0x12);

  // Start Measurements
  sensor_vl53lx_sat.VL53LX_StartMeasurement();
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
    VL53LX_MultiRangingData_t MultiRangingData;
    VL53LX_MultiRangingData_t* pMultiRangingData = &MultiRangingData;
    uint8_t NewDataReady = 0;
    int no_of_object_found = 0, j;
    char report[64];
    int status;

    if (hms->data.logLevel >= 2) Serial.println("before");
    do
    {
        status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
    } while (!NewDataReady);
    if (hms->data.logLevel >= 2) Serial.println("after");

    if ((!status) && (NewDataReady != 0))
    {
        status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
        Serial.print("Status: ");
        Serial.print(status);
        no_of_object_found = pMultiRangingData->NumberOfObjectsFound;
        snprintf(report, sizeof(report), " VL53LX Satellite: Count=%d, #Objs=%1d ", pMultiRangingData->StreamCount, no_of_object_found);
        Serial.print(report);
        for (j = 0; j < no_of_object_found; j++)
        {

            tofData.dist = pMultiRangingData->RangeData[j].RangeMilliMeter;

            //if(pMultiRangingData->RangeData[j].RangeStatus == 0){
            Serial.print("status=");
            Serial.print(pMultiRangingData->RangeData[j].RangeStatus);
            Serial.print(", D=");
            Serial.print(pMultiRangingData->RangeData[j].RangeMilliMeter);
            Serial.print("mm");
            Serial.print(", Signal=");
            Serial.print((float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps / 65536.0);
            Serial.print(" Mcps, Ambient=");
            Serial.print((float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps / 65536.0);
            Serial.print(" Mcps");
            //}

        }
        Serial.println("");
        if (status == 0)
        {
            status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
        }
        NewDataReady = 0;
    }
  return;
}
