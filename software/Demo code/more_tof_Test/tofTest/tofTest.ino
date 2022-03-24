#include <Arduino.h>
#include <Wire.h>
#include "vl53lx_class.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tof.h"

#define MUX_S1 27
#define MUX_S2 14

#define TOF_1_PIN 19
#define TOF_2_PIN 18

uint8_t tofAddresses[2] = {0x12, 0x13};


VL53LX sensor[2] = {
    VL53LX(&Wire, TOF_1_PIN), //2nd arg is shutdown pin
    VL53LX(&Wire, TOF_2_PIN), //2nd arg is shutdown pin
};
Tof tof[2];

void setup()
{
    // Initialize serial for output.
    Serial.begin(115200);
    Serial.println("Starting...");


    Wire.begin();
    Wire.setClock(400000);
    pinMode(TOF_1_PIN, OUTPUT);
    pinMode(TOF_2_PIN, OUTPUT);

    digitalWrite(TOF_1_PIN, LOW);
    digitalWrite(TOF_2_PIN, LOW);
    delay(1000);

  for (int i=0; i<2; i++){
      tof[i] = Tof(&sensor[i], tofAddresses[i]);  // initialize tof object (driver) with a pointer to sensor (physical device)
      delay(30);
  }

    Serial.println("Initialized well");
    delay(3000);
}

void loop()
{
//     Serial.print(".");
    for (int i = 0; i < 2; ++i)
    {
        tof[i].poll();
        Serial.print(i);
        Serial.print("; ");
        Serial.print(tof[i].getData().dist);
        Serial.print("; ");
        Serial.print(tof[i].getData().numObjs);
        Serial.print("; ");
        Serial.print(tof[i].getData().count);
        Serial.print("; ");
        Serial.print(tof[i].getData().timeoutCount);
        Serial.print("; ");
    }
    Serial.println();
}
