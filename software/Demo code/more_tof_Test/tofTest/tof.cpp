#include "tof.h"

#define TIMEOUT 50000
#define MAX_TIMEOUTS 3
#define MAX_BAD_READINGS 5

/* Tof::Tof(){ */
/* } */
Tof::Tof()
{
}

bool Tof::init(){

  bool initializedProperly = true;
  // if (print) Serial.println("begin");

  // Configure VL53LX satellite component.
  sensor_vl53lx_sat->begin();

  // if (print) Serial.println("init tof sensor");
  //the dumb library doesn't understand that if you reboot the device, it goes back to the default address, so we need to remind it.
  sensor_vl53lx_sat->VL53LX_SetDeviceAddress(VL53LX_DEFAULT_DEVICE_ADDRESS);
  //Initialize VL53LX satellite component.
  initializedProperly &= sensor_vl53lx_sat->InitSensor(0x10 + index*2) == 0;  // ensure sensor initialized properly


  // if (print) Serial.println("start measurement");
  // Start Measurements
  sensor_vl53lx_sat->VL53LX_StartMeasurement();

  needsToBeInitialized = false;

  return initializedProperly;
}

Tof::Tof(VL53LX* tof_sensor, uint8_t tof_index):
  sensor_vl53lx_sat(tof_sensor)
{
  tofData = {0, 0, 0, 0};
  index = tof_index;
  init();
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
    VL53LX_MultiRangingData_t MultiRangingData;
    VL53LX_MultiRangingData_t* pMultiRangingData = &MultiRangingData;

    sensor_vl53lx_sat->VL53LX_GetMeasurementDataReady(&NewDataReady);  // TODO: what if status bad
    if(NewDataReady == 1){
        NewDataReady = 0;
//        if (print) Serial.println("ready");

        if (status == 0)
        {
            status = sensor_vl53lx_sat->VL53LX_GetMultiRangingData(pMultiRangingData);
            if (status == 0) {
                lastReading = micros();
                tofData.numObjs = pMultiRangingData->NumberOfObjectsFound;

                if (tofData.numObjs){ // if at least 1 object found
                    if (pMultiRangingData->RangeData[0].RangeStatus == 0){
                        consecutiveBadReadings = 0;
                        tofData.dist = pMultiRangingData->RangeData[0].RangeMilliMeter;
                        tofData.count = pMultiRangingData->StreamCount;
                        lastReading = micros();
                    } else{
                        consecutiveBadReadings++;
                        if (consecutiveBadReadings % MAX_BAD_READINGS == 0){
                            needsToBeInitialized = true;
                            Serial.println("REBOOTING TOF cause consecutiveBadReadings");
                            init();
                            return;
                        }
                    } // TODO: if no objects found, we don't increment consecutiveBadReadings

                    if (print) {
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
        if (print) Serial.println("Timeout");
        status = sensor_vl53lx_sat->VL53LX_ClearInterruptAndStartMeasurement(); // TODO: what if status bad
        NewDataReady = 0;
        tofData.timeoutCount++;


        if (tofData.timeoutCount % MAX_TIMEOUTS == 0){
          needsToBeInitialized = true;
          Serial.println("REBOOTING TOF cause timeout");
          Serial.println(init());
          return;
        }
    }
}
