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


VL53LX sensor(&Wire, PLACEHOLDER_PIN); //2nd arg is shutdown pin (use placeholder)
Tof tof;

#define NUM_TO_READ 1000

TofData data[NUM_TO_READ];
uint32_t delT[NUM_TO_READ];
int readingsTillNew[NUM_TO_READ];

int currNum = 0;
int currReadingsTillNew = 0;
uint32_t prevTime;
int prevCount = -1;

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

    tof = Tof(&sensor);

    delay(3000);

    prevTime = micros();

}

void loop()
{
    if (currNum < NUM_TO_READ){
        tof.poll();
        currReadingsTillNew++;
        if(tof.getData().count != prevCount){
            uint32_t currTime = micros();
            delT[currNum] = currTime - prevTime;
            prevTime = currTime;
            data[currNum] = tof.getData();
            readingsTillNew[currNum] = currReadingsTillNew;
            currNum++;
            prevCount = tof.getData().count;
        }
    }
    else{
        for (int i = 0; i < NUM_TO_READ; ++i){
            Serial.print(delT[i]);
            Serial.print(";");
            Serial.print(data[i].dist);
            Serial.print(";");
            Serial.print(data[i].numObjs);
            Serial.print(";");
            Serial.print(data[i].count);
            Serial.print(";");
            Serial.print(data[i].timeoutCount);
            Serial.print(";");
            Serial.print(readingsTillNew[i]);
            Serial.println();
        }
        while(1);
    }
}
