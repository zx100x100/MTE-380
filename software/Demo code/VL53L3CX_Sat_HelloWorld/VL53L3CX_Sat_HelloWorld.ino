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



VL53LX sensor_vl53lx_sat(&Wire, 4); //2nd arg is shutdown pin (use placeholder)

#define TCAADDR 0x70
#define MUX_ADDR 3
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

   Wire.beginTransmission(TCAADDR);
   Wire.write(1 << MUX_ADDR);
   Wire.endTransmission();  
   delay(100);

   // Configure VL53LX satellite component.
   sensor_vl53lx_sat.begin();


   //Initialize VL53LX satellite component.
   sensor_vl53lx_sat.InitSensor(0x12);

   // Start Measurements
   sensor_vl53lx_sat.VL53LX_StartMeasurement();

   pinMode(2, OUTPUT);
   pinMode(4, OUTPUT);
   digitalWrite(2, LOW);
   digitalWrite(4, LOW);
}

void loop()
{
   VL53LX_MultiRangingData_t MultiRangingData;
   VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
   uint8_t NewDataReady = 0;
   int no_of_object_found = 0, j;
   char report[64];
   int status;

   do
   {
      status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);
   } while (!NewDataReady);

   if((!status)&&(NewDataReady!=0))
   {
      status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
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
         status = sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
      }
      NewDataReady = 0;
   }
}
