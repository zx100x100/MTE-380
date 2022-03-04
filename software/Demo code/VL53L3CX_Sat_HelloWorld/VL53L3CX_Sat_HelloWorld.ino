/*
 * To use this sketch you need to connect the VL53L3CX satellite sensor directly to the Nucleo board with wires in this way:
 * pin 1 (Interrupt) of the VL53L3CX satellite connected to pin A2 of the Nucleo board 
 * pin 2 (SCL_I) of the VL53L3CX satellite connected to pin D15 (SCL) of the Nucleo board with a Pull-Up resistor of 4.7 KOhm
 * pin 3 (XSDN_I) of the VL53L3CX satellite connected to pin A1 of the Nucleo board
 * pin 4 (SDA_I) of the VL53L3CX satellite connected to pin D14 (SDA) of the Nucleo board with a Pull-Up resistor of 4.7 KOhm
 * pin 5 (VDD) of the VL53L3CX satellite connected to 3V3 pin of the Nucleo board
 * pin 6 (GND) of the VL53L3CX satellite connected to GND of the Nucleo board
 * pins 7, 8, 9 and 10 are not connected.
 */
/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>
#include "vl53lx_class.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PLACEHOLDER_PIN 0

#define TCAADDR 0x70

#define FRONT_TOF 4
#define REAR_TOF 5
#define L_BACK_TOF 3
#define L_FRONT_TOF 2

uint8_t mux_addresses[4] = { FRONT_TOF, REAR_TOF, L_BACK_TOF, L_FRONT_TOF };

VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, PLACEHOLDER_PIN),
  VL53LX(&Wire, PLACEHOLDER_PIN),
  VL53LX(&Wire, PLACEHOLDER_PIN),
  VL53LX(&Wire, PLACEHOLDER_PIN)
};

/*
void TOF_ select(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}
*/
/* Setup ---------------------------------------------------------------------*/

void setup()
{

   // Initialize serial for output.
   Serial.begin(115200);
   Serial.println("Starting...");

   // Initialize I2C bus.
   Wire.begin();

   for (int i = 0; i < 4; ++i){
     Wire.beginTransmission(TCAADDR);
     Wire.write(1 << mux_addresses[i]);
     Wire.endTransmission();  
     delay(100);
    
     //Initialize VL53LX satellite component.
     do{
        Serial.print("Initializing TOF "); Serial.println(i);
        delay(5);
     }while(sensor_vl53lx_sat[i].InitSensor(0x12));

     // Start Measurements
     do{
        Serial.print("Starting measurement TOF "); Serial.println(i);
        delay(5);
     }while(sensor_vl53lx_sat[i].VL53LX_StartMeasurement());
  
   }
}

void loop()
{
  for (int tof_num = 0; tof_num < 4; ++tof_num){

     Wire.beginTransmission(TCAADDR);
     Wire.write(1 << mux_addresses[tof_num]);
     Wire.endTransmission();
    
     VL53LX_MultiRangingData_t MultiRangingData;
     VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
     uint8_t NewDataReady = 0;
     int no_of_object_found = 0, j;
     char report[64];
     int status;
  
     do
     {
        status = sensor_vl53lx_sat[tof_num].VL53LX_GetMeasurementDataReady(&NewDataReady);
     } while (!NewDataReady);
  
     if((!status)&&(NewDataReady!=0))
     {
        status = sensor_vl53lx_sat[tof_num].VL53LX_GetMultiRangingData(pMultiRangingData);
        Serial.print("Status: ");
        Serial.print(status);
        no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
        snprintf(report, sizeof(report), " VL53LX Satellite: Count=%d, #Objs=%1d ", pMultiRangingData->StreamCount, no_of_object_found);
        Serial.print(report);
        for(j=0;j<no_of_object_found;j++)
        {
  
           //if(pMultiRangingData->RangeData[j].RangeStatus == 0){
             Serial.print("status=");
             Serial.print(pMultiRangingData->RangeData[j].RangeStatus);
             Serial.print(", D=");
             Serial.print(pMultiRangingData->RangeData[j].RangeMilliMeter);
             Serial.print("mm");
             Serial.print(", Signal=");
             Serial.print((float)pMultiRangingData->RangeData[j].SignalRateRtnMegaCps/65536.0);
             Serial.print(" Mcps, Ambient=");
             Serial.print((float)pMultiRangingData->RangeData[j].AmbientRateRtnMegaCps/65536.0);
             Serial.print(" Mcps");
           //}
           
        }
        Serial.println("");
        if (status==0)
        {
           status = sensor_vl53lx_sat[tof_num].VL53LX_ClearInterruptAndStartMeasurement();
        }
        NewDataReady = 0;
     }
  }
}
