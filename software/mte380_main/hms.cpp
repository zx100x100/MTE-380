#include "hms.h"
#include "log_level_defaults.h"

#define DEFAULT_N_BATTERY_CELLS 2
#define LED_PIN_GREEN 32
#define LED_PIN_RED 33
#define LED_SLOW_FLASH_TICKS_INTERVAL 40
#define LED_FAST_FLASH_TICKS_INTERVAL 20

Hms::Hms()
{
  navData = NavData_init_zero;
  HmsData data = HmsData_init_zero;
  /* HmsData data_ = HmsData_init_zero; */
  /* data = data_; */
  data.nCells = DEFAULT_N_BATTERY_CELLS;
  data.nTicks = 0;
  greenLedState = LED_FAST_FLASH;
  greenLedLastOn = false;
  redLedState = LED_ON;
  redLedLastOn = false;
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_RED, OUTPUT);
}

void Hms::updateLEDs(){
  if (greenLedState == LED_ON){
    if (!greenLedLastOn){
      digitalWrite(LED_PIN_GREEN, 1);
    }
    greenLedLastOn = true;
  }
  else if (greenLedState == LED_SLOW_FLASH){
    if (data.nTicks % LED_SLOW_FLASH_TICKS_INTERVAL == 0){
      if (greenLedLastOn){
        digitalWrite(LED_PIN_GREEN, 0);
      }
      else{
        digitalWrite(LED_PIN_GREEN, 1);
      }
      greenLedLastOn = !greenLedLastOn;
    }
  }
  else{
    if (data.nTicks % LED_FAST_FLASH_TICKS_INTERVAL == 0){
      if (greenLedLastOn){
        digitalWrite(LED_PIN_GREEN, 0);
      }
      else{
        digitalWrite(LED_PIN_GREEN, 1);
      }
      greenLedLastOn = !greenLedLastOn;
    }
  }
  if (redLedState == LED_ON){
    if (!redLedLastOn){
      digitalWrite(LED_PIN_RED, 1);
    }
    redLedLastOn = true;
  }
  else if (redLedState == LED_SLOW_FLASH){
    if (data.nTicks % LED_SLOW_FLASH_TICKS_INTERVAL == 0){
      if (redLedLastOn){
        digitalWrite(LED_PIN_RED, 0);
      }
      else{
        digitalWrite(LED_PIN_RED, 1);
      }
      redLedLastOn = !redLedLastOn;
    }
  }
  else{
    if (data.nTicks % LED_FAST_FLASH_TICKS_INTERVAL == 0){
      if (redLedLastOn){
        digitalWrite(LED_PIN_RED, 0);
      }
      else{
        digitalWrite(LED_PIN_RED, 1);
      }
      redLedLastOn = !redLedLastOn;
    }
  }
}

void Hms::init(){
  data.mainLogLevel = MAIN_LOG_LEVEL_DEFAULT;
  data.sensorsLogLevel = SENSORS_LOG_LEVEL_DEFAULT;
  data.guidanceLogLevel = GUIDANCE_LOG_LEVEL_DEFAULT;
  data.navLogLevel = NAV_LOG_LEVEL_DEFAULT;
}

void Hms::update(){
  data.nTicks++;

  updateLEDs();
}

/* void Hms::logError(int err, char str[80]){ */
  /* Serial.print("Error: "); Serial.print(err); Serial.println(str); */
/* } */
