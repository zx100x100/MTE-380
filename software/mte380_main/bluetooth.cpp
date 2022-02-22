#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define bleServerName "EMU"

#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"


// Temperature Characteristic and Descriptor
BLECharacteristic valueOneCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor valueOneDescriptor(BLEUUID((uint16_t)0x2902));

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

bool deviceConnected = false;

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

/* void initBME(){ */
  /* if (!bme.begin(0x76)) { */
    /* Serial.println("Could not find a valid BME280 sensor, check wiring!"); */
    /* while (1); */
  /* } */
/* } */

void bluetooth_setup() {
  // Start serial communication 
  // Init BME Sensor
  /* initBME(); */

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *bmeService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and Create a BLE Descriptor
  // Temperature
  bmeService->addCharacteristic(&valueOneCharacteristics);
  valueOneDescriptor.setValue("Something");
  valueOneCharacteristics.addDescriptor(&valueOneDescriptor);

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  /* Serial.println("Waiting a client connection to notify..."); */
}

bool bluetooth_loop(unsigned long millis) {
  if (deviceConnected) {
    if ((millis - lastTime) > timerDelay) {
      /* value = 1.35 */
  
      static char valueOneTemp[6] = {'a','b','c','d','e','f'};
      /* dtostrf(value, 6, 2, valueOneTemp); */
      //Set temperature Characteristic value and notify connected client
      valueOneCharacteristics.setValue(valueOneTemp);
      valueOneCharacteristics.notify();
      /* Serial.print("Value: "); */
      /* Serial.print(temp); */
      /* Serial.println(""); */
      
      lastTime = millis;
    }
  }
  return deviceConnected;
}
