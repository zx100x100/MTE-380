#include "tof.h"

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
    pinMode(19, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    digitalWrite(19, LOW);
    digitalWrite(18, LOW);
    digitalWrite(25, LOW);
    digitalWrite(26, LOW);
    delay(150);
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

//     tof = Tof(static_cast<VL53LX*>(&sensor), TOF_INDEX);
//     Serial.println("Started");
    /* we create a new task here */
    xTaskCreate(
        anotherTask, /* Task function. */
        "another Task", /* name of task. */
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
    if ((micros() - lastReading)/1000 > 250){
        Serial.println("Watchdog go brrrrrrrr");
        vTaskDelete(xHandle);
        Serial.println("We killed it");

        xTaskCreate(
            anotherTask, /* Task function. */
            "another Task", /* name of task. */
            10000, /* Stack size of task */
            &sensor, /* parameter of the task */
            1, /* priority of the task */
            &xHandle
        ); /* Task handle to keep track of created task */
        Serial.println("made another task");
    }
    delay(1000);
}

/* this function will be invoked when additionalTask was created */
void anotherTask( void * sensorPtr )
{
    tof = Tof(static_cast<VL53LX*>(sensorPtr), TOF_INDEX);
    tof.print = false;
    Serial.println("Started");
    /* loop forever */
    for(;;)
    {
        Serial.println("tof poll");
        tof.poll();
        if (tof.getData().count != prevCount){
            Serial.printf("Count: %d, Dist: %d, numObjs: %d, timeoutCount: %d\n",
            tof.getData().count, tof.getData().dist, tof.getData().numObjs, tof.getData().timeoutCount);
            prevCount = tof.getData().count;
            lastReading = micros();
        }
        delay(5);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}
