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

#define TOF_SHUTDOWN_PIN 18
#define PLACEHOLDER_PIN 19 // placeholder pin for the tof to power cycle. Never used. make sure this is empty

#define OH_SHIT_COUNT_BAD 10000


VL53LX sensor[4] = {
    VL53LX(&Wire, PLACEHOLDER_PIN), //2nd arg is shutdown pin (use placeholder)
    VL53LX(&Wire, PLACEHOLDER_PIN), //2nd arg is shutdown pin (use placeholder)
    VL53LX(&Wire, PLACEHOLDER_PIN), //2nd arg is shutdown pin (use placeholder)
    VL53LX(&Wire, PLACEHOLDER_PIN) //2nd arg is shutdown pin (use placeholder)
};
Tof tof[4];

#define NUM_TO_READ 500

TofData data[4][NUM_TO_READ] = {};
uint32_t delT[4][NUM_TO_READ] = {};
int readingsTillNew[4][NUM_TO_READ] = {};

int currNum[4] = {0, 0, 0, 0};
int currReadingsTillNew[4] = {0, 0, 0, 0};
uint32_t prevTime[4];
int prevCount[4] = {-1, -1, -1, -1};


uint8_t mux_addresses[4] = {2, 1, 0, 3};

void setup()
{
    // Initialize serial for output.
    Serial.begin(115200);
    Serial.println("Starting...");


    Wire.begin();
    Wire.setClock(400000);
    pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT);

    // power cycle the TOFs to initialize them
    pinMode(TOF_SHUTDOWN_PIN, OUTPUT);
    digitalWrite(TOF_SHUTDOWN_PIN, LOW);
    delay(10);
    digitalWrite(TOF_SHUTDOWN_PIN, HIGH);
    delay(10);

  for (int i=0; i<4; i++){
      //    Serial.println("Starting mux shit");
      digitalWrite(MUX_S1, mux_addresses[i]&0x01);  //  mux the first
      digitalWrite(MUX_S2, (mux_addresses[i]&0x02)>>1);  /// mux the second
      delay(100);
      tof[i] = Tof(&sensor[i]);  // initialize tof object (driver) with a pointer to sensor (physical device)
  }

    Serial.println("Initialized well");
    delay(3000);

    for (int i  = 0; i < 4; ++i){
        prevTime[i] = micros();
    }
}

void loop()
{
//     Serial.print(".");
    for (int i = 0; i < 4; ++i)
    {
        if (currNum[i] < NUM_TO_READ){
            digitalWrite(MUX_S1, mux_addresses[i]&0x01);  //  mux the first
            digitalWrite(MUX_S2, (mux_addresses[i]&0x02)>>1);  /// mux the second
            tof[i].poll();

            currReadingsTillNew[i]++;
            if(tof[i].getData().count != prevCount[i]){   // do data logging if count changed
                uint32_t currTime = micros();
                delT[i][currNum[i]] = currTime - prevTime[i];
                prevTime[i] = currTime;
                data[i][currNum[i]] = tof[i].getData();
                readingsTillNew[i][currNum[i]] = currReadingsTillNew[i];
                currNum[i]++;
                prevCount[i] = tof[i].getData().count;
            }
        }
    }
    bool notFinished = true;
    for (int i = 0; i < 4; i++){
        notFinished &= currNum[i] < NUM_TO_READ && currReadingsTillNew[i] <= OH_SHIT_COUNT_BAD;
    }
    if (!notFinished){
        for (int i = 0; i < 4; ++i){
            Serial.print("Tof "); Serial.print(i); Serial.print(" currReadingsTillNew: "); Serial.println(currReadingsTillNew[i]);
        }
        for (int i = 0; i < 4; ++i) {
            Serial.print("delT;dist;numObjs;count;timeoutCount;readingsTillNew;;;");
        }
        Serial.println();
        for (int j = 0; j < NUM_TO_READ; j++){
            for (int i = 0; i < 4; ++i){
                Serial.print(delT[i][j]);
                Serial.print(";");
                Serial.print(data[i][j].dist);
                Serial.print(";");
                Serial.print(data[i][j].numObjs);
                Serial.print(";");
                Serial.print(data[i][j].count);
                Serial.print(";");
                Serial.print(data[i][j].timeoutCount);
                Serial.print(";");
                Serial.print(readingsTillNew[i][j]);
                Serial.print(";;;");
            }
            Serial.println();
        }
        while(1);
    }
}
