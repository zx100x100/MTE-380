#ifndef HMS_H
#define HMS_H

#include "hms_and_cmd_data.pb.h"
#include "Arduino.h"

enum LedState{LED_ON, LED_SLOW_FLASH, LED_FAST_FLASH, LED_OFF};

class Hms{
  public:
    Hms();
    void init();
    void update();
    void logError(int err, char str[80]);
    HmsData data;
    LedState greenLedState;
    bool greenLedLastOn;
    LedState redLedState;
    bool redLedLastOn;
    void updateLEDs();
};

#endif
