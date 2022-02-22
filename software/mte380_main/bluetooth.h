#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer);
  void onDisconnect(BLEServer* pServer);
};

// Timer variables
// unsigned long lastTime;
// unsigned long timerDelay;

// bool deviceConnected;

void bluetooth_setup();
bool bluetooth_loop(unsigned long millis);

#endif
