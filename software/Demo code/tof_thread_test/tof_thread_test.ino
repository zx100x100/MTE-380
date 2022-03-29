#include "tof.h"
/* #include "FreeRTOSConfig.h" */

#define TOF_X_SHUT 19
#define TOF_INDEX 0

VL53LX sensor = VL53LX(&Wire, TOF_X_SHUT);
Tof tof;
int prevCount = -1;
uint32_t lastReading = 0;
TaskHandle_t xHandle = NULL;

volatile TofData tofData;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    Serial.println("Started");
    xTaskCreate(
        tofTask, /* Task function. */
        "tof Task", /* name of task. */
        10000, /* Stack size of task */
        &sensor, /* parameter of the task */
        1, /* priority of the task */
        &xHandle
    ); /* Task handle to keep track of created task */
    delay(250);
}

/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop()
{
    Serial.println("this is ESP32 Task");

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 100;

    xLastWakeTime = xTaskGetTickCount();

    while(true){
        Serial.print("wakeTime: "); Serial.println(xLastWakeTime);
        /* continue; */
        /* if ((micros() - lastReading)/1000 > 250){ */
        vTaskDelayUntil( &xLastWakeTime, xFrequency );


        // you have this
        // you needed this
        if ((micros() - lastReading)/1000 > 150){

            Serial.println("Watchdog go brrrrrrrr");
            vTaskDelete(xHandle);
            Serial.println("We killed it");

            xTaskCreate(
                tofTask, /* Task function. */
                "tof Task", /* name of task. */
                10000, /* Stack size of task */
                &sensor, /* parameter of the task */
                1, /* priority of the task */
                &xHandle
            ); /* Task handle to keep track of created task */
            Serial.println("made another task");
        }
    }
}

/* this function will be invoked when additionalTask was created */
void tofTask( void * sensorPtr )
{
    tof = Tof(static_cast<VL53LX*>(sensorPtr), TOF_INDEX);
    /* tof.print = false; */
    Serial.println("Started");
    /* loop forever */
    for(;;){
        lastReading = micros();
        Serial.printf("tof poll: %lu\n", lastReading);

        tof.poll();
        if (tof.getData().count != prevCount){
            Serial.printf("Count: %d, Dist: %d, numObjs: %d, timeoutCount: %d\n",
            tof.getData().count, tof.getData().dist, tof.getData().numObjs, tof.getData().timeoutCount);
            prevCount = tof.getData().count;
        }
        delay(5);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}
