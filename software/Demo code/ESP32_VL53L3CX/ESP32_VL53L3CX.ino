/* To run:
Install the STM32duino VL53L3CX Library
If you’re gonna use ESP32 to drive VL53L3CX (Which you are), you need to change the I2C buffer size in the following way:

Enter the file Directory
%USERPROFILE%\AppData\Local\Arduino15\packages\DFRobot\hardware\esp32\0.2.1\libraries\Wire\src

Open the Wire.h file

Change the I2C_BUFFER_LENGTH macro value to 256
 */


//--------------------------------------------------------------------------------
// ESP32 & VL53L3CX test app by Motti Bazar.
// App utilizes the interrupt method.
// Code parts below that were developed by me can be used for non-commercial use
// but I am asking to be mentioned as the developer.
//
// 5-May-2021
//
// My ESP32 setup:
//  SDA   - pin 21
//  SCL   - pin 22
//  INT   - pin 32
//  XSHUT - pin 33
//--------------------------------------------------------------------------------

#include <Arduino.h>
#include <Wire.h>
#include <vl53lx_class.h>

#define  XSHUT_PIN  33
#define  INT_PIN    32
#define  SERIAL_BAUD_RATE  115200

#define  DEVICE_I2C_ADDR  0x12
VL53LX vl53lx(&Wire, XSHUT_PIN);

// Holds latest measured distances (up to 4)
#define  MAX_OBJECTS  4
uint16_t dist[MAX_OBJECTS];


//----------------------------------------
// intaISR - Interrupt handler
//----------------------------------------
portMUX_TYPE  syncINTA = portMUX_INITIALIZER_UNLOCKED;        // For handling ISR entry/exit
bool          intaINT  = false;                               // True = Interrupt occured
unsigned long intervalTimer;                                  // Timer used for checking for lost interrupts

void IRAM_ATTR intaISR() {
  portENTER_CRITICAL(&syncINTA);
  intaINT = true;                                             // Signal that MCP INTA occured
  portEXIT_CRITICAL(&syncINTA);
}


void setup()
{
   int i;
   VL53LX_Error rc;
   VL53LX_Version_t ver;
   uint8_t ProductRevisionMajor, ProductRevisionMinor, byteData;
   VL53LX_DeviceInfo_t devInfo;
   uint64_t Uid;

   Serial.begin(SERIAL_BAUD_RATE, SERIAL_8N1);
   while (!Serial);
   delay(5000);
   Serial.println("Starting...");

   // Initialize the array
   for (i=0; i<MAX_OBJECTS; i++)
      dist[i] = 0;

   // Initialize the I2C bus
   Wire.begin();
   Wire.setClock(400000);

   // Configure the VL53LX
   vl53lx.begin();
   vl53lx.VL53LX_Off();
   vl53lx.InitSensor(DEVICE_I2C_ADDR);

   // Print chip info
   rc = vl53lx.VL53LX_GetVersion(&ver);
   Serial.printf("GetVersion: rc=%d, Build=%d, Major=%d, Minor=%d, Revision=%d\n", rc, ver.build, ver.major, ver.minor, ver.revision);
   rc = vl53lx.VL53LX_GetProductRevision(&ProductRevisionMajor, &ProductRevisionMinor);
   Serial.printf("GetProductRevision: rc=%d, Major=%d, Minor=%d\n", rc, ProductRevisionMajor, ProductRevisionMinor);
   rc = vl53lx.VL53LX_GetDeviceInfo(&devInfo);
   Serial.printf("GetDeviceInfo: rc=%d, ModuleType(0xAA?)=0x%X, ProdRevMajor=%d, ProdRevMinor=%d\n", rc, devInfo.ProductType, devInfo.ProductRevisionMajor, devInfo.ProductRevisionMinor);
   rc = vl53lx.VL53LX_GetUID(&Uid);
   Serial.printf("GetUID: rc=%d, UID=0x%llX\n", rc, Uid);

   // To enable using the below I2CRead function, you need to move its definition in vl53lx_class.h from the Protected section to the Public section
   rc = vl53lx.VL53LX_I2CRead(DEVICE_I2C_ADDR, 0x010F, &byteData, 1);
   Serial.printf("I2CRead 0x010F: rc=%d, Model_ID=0x%02X\n", rc, byteData);
   rc = vl53lx.VL53LX_I2CRead(DEVICE_I2C_ADDR, 0x0110, &byteData, 1);
   Serial.printf("I2CRead 0x0110: rc=%d, Module_Type=0x%02X\n", rc, byteData);

   pinMode(INT_PIN, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(INT_PIN), intaISR, FALLING);
   intervalTimer = millis();                                            // Set timer

   vl53lx.VL53LX_ClearInterruptAndStartMeasurement();
   Serial.println("Ready...");
}


void loop()
{
   VL53LX_MultiRangingData_t MultiRangingData;
   VL53LX_MultiRangingData_t *rangingData = &MultiRangingData;
   uint8_t NewDataReady = 0;
   int objectsFound = 0, j, status;
   bool dataChanged = false;

   if (intaINT) {

      status = vl53lx.VL53LX_GetMeasurementDataReady(&NewDataReady);
      if (status == 0  &&  NewDataReady != 0) {

         status       = vl53lx.VL53LX_GetMultiRangingData(rangingData);
         objectsFound = rangingData->NumberOfObjectsFound;
         if (objectsFound > 0) {

            for(j = 0; j < objectsFound; j++) {
               // Check only if status is OK
               if (rangingData->RangeData[j].RangeStatus == 0) {
                  // Only consider changes of over 10 mm
                  if (abs(rangingData->RangeData[j].RangeMilliMeter - dist[j]) > 10) {
                     // Movement happened
                     dist[j] = rangingData->RangeData[j].RangeMilliMeter;
                     dataChanged = true;
                  }
               }
            }
            // Clear rest of values
            for (j=objectsFound; j < MAX_OBJECTS; j++)
               dist[j] = 0;

            if (dataChanged) {
               dataChanged = false;
               Serial.printf("%d %d %d %d\n", dist[0], dist[1], dist[2], dist[3]);
            }
         }
      }

      intaINT = false;
      vl53lx.VL53LX_ClearInterruptAndStartMeasurement();
      intervalTimer = millis();

   } else {

      // Did we lose an interrupt?

     if ((millis() - intervalTimer) > 2000) {

         Serial.println("Lost interrupt?");
         vl53lx.VL53LX_ClearInterruptAndStartMeasurement();
         intervalTimer = millis();
      }
   }

}
