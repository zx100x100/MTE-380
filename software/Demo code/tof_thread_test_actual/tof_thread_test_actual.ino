#include "tof.h"
#include <Arduino.h>
#include "Esp32SynchronizationContext.h"

#define TOF_X_SHUT 19
#define TOF_INDEX 0

VL53LX sensor = VL53LX(&Wire, TOF_X_SHUT);
Tof tof;
int prevCount = -1;
volatile uint32_t lastReading = 0;
TaskHandle_t xHandle = NULL;

volatile TofData tofData;

Esp32SynchronizationContext g_mainSync;

void setup()
{
    Serial.begin(112500);
    Wire.begin();
    Wire.setClock(400000);

//     tof = Tof(static_cast<VL53LX*>(&sensor), TOF_INDEX);
//     Serial.println("Started");
    /* we create a new task here */


    if(!g_mainSync.begin()) {
        Serial.println("Error initializing synchronization context");
        while(true);          // halt
    }


    xTaskCreate(
        mainThread, /* Task function. */
        "main thread", /* name of task. */
        10000, /* Stack size of task */
        NULL, /* parameter of the task */
        1, /* priority of the task */
        NULL
    ); /* Task handle to keep track of created task */

    xTaskCreate(
        tofThread, /* Task function. */
        "tof Task", /* name of task. */
        10000, /* Stack size of task */
        NULL, /* parameter of the task */
        1, /* priority of the task */
        &xHandle
    ); /* Task handle to keep track of created task */

    delay(250);
}

/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop()
{
    if(!g_mainSync.update()) {
        Serial.println("Could not update synchronization context");
    }
}

void mainThread(void * state) {
    while(g_mainSync.post([](void * state){
                                            Serial.println("main");
                                            if ((micros() - lastReading)/1000 > 250){
                                                Serial.println("Watchdog go brrrrrrrr");
                                                vTaskDelete(xHandle);
                                                Serial.println("We killed it");

                                                xTaskCreate(
                                                    tofThread, /* Task function. */
                                                    "Tof Task", /* name of task. */
                                                    10000, /* Stack size of task */
                                                    NULL, /* parameter of the task */
                                                    1, /* priority of the task */
                                                    &xHandle
                                                ); /* Task handle to keep track of created task */
                                                Serial.println("made another task");
                                            }
    })) {
        vTaskDelay(1000);
    }

    vTaskDelete( NULL );
}

/* this function will be invoked when additionalTask was created */
void tofThread( void * state )
{
    tof = Tof(static_cast<VL53LX*>(&sensor), TOF_INDEX);
    tof.print = false;
    Serial.println("Started");
    /* loop forever */
    while(g_mainSync.post([](void* state){
                                        Serial.println("tof poll");
                                        tof.poll();
                                        if (tof.getData().count != prevCount){
                                            Serial.printf("Count: %d, Dist: %d, numObjs: %d, timeoutCount: %d\n",
                                            tof.getData().count, tof.getData().dist, tof.getData().numObjs, tof.getData().timeoutCount);
                                            prevCount = tof.getData().count;
                                            lastReading = micros();
                                        }
        }))
    {
        vTaskDelay(5);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}
