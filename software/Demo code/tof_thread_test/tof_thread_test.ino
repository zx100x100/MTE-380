#include "tof.h"
/* #include "FreeRTOSConfig.h" */

#define TOF_X_SHUT 19
#define TOF_INDEX 0

#define NUM_TOF_TO_USE 1

#define RUN_TIMEOUT 5000 //ms
#define START_DELAY 4000 //ms

VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, tofPins[0]),
  VL53LX(&Wire, tofPins[1]),
  VL53LX(&Wire, tofPins[2]),
  VL53LX(&Wire, tofPins[3])
};


Tof tof[4];

int prevCount = -1;
uint32_t lastReading = 0;
TaskHandle_t xHandle = NULL;

// const static int tofPins[4] = {26, 25, 18, 19};

volatile TofData tofData;

void setup()
{
    Serial.begin(115200);
    delay(5000);

    Wire.begin();
    Wire.setClock(400000);

    Serial.print("sensor Ptr: "); Serial.printf("%lu\n", (void*)&sensor_vl53lx_sat[0]);



  for (int i = 0; i < 4; ++i){
    pinMode(tofPins[i], OUTPUT);
    digitalWrite(tofPins[i], LOW);
  }

    Serial.println("Started");
    xTaskCreate(
        tofTask, /* Task function. */
        "tof Task", /* name of task. */
        10000, /* Stack size of task */
        &sensor_vl53lx_sat[3], /* parameter of the task */
        1, /* priority of the task */
        &xHandle
    ); /* Task handle to keep track of created task */
    delay(250);
}

/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop()
{
    while(true){
        delay(1000);
        Serial.println("this is ESP32 Task");
        if ((micros() - lastReading)/1000 > 1000000){

            Serial.println("Watchdog go brrrrrrrr");
            vTaskDelete(xHandle);
            Serial.println("We killed it");

            xTaskCreate(
                tofTask, /* Task function. */
                "tof Task", /* name of task. */
                10000, /* Stack size of task */
                &sensor_vl53lx_sat[3], /* parameter of the task */
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
    Wire.begin();
    Serial.print("sensor Ptr: "); Serial.printf("%lu\n", (void*)sensorPtr);
    tof[0] = Tof(static_cast<VL53LX*>(sensorPtr), TOF_INDEX);
    Serial.println("Initialized");

    while(millis() < START_DELAY);
    delay(10);
    /* tof.print = false; */
    Serial.println("Started");
    /* loop forever */
    for(;;){
        lastReading = micros();
        Serial.printf("tof poll: %lu\n", lastReading);

        tof[0].poll();
        if (tof[0].getData().count != prevCount){
            Serial.printf("Count: %d, Dist: %d, numObjs: %d, timeoutCount: %d\n",
            tof[0].getData().count, tof[0].getData().dist, tof[0].getData().numObjs, tof[0].getData().timeoutCount);
            prevCount = tof[0].getData().count;
        }
        delay(5);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}
