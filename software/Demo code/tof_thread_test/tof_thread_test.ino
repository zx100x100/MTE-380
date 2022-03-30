#include "tof.h"
#include "FreeRTOSConfig.h"

#define RUN_TIMEOUT 5000 //ms
#define START_DELAY 4000 //ms

struct TofInfo {
  VL53LX* sensor;
  TofData tofData;
  uint8_t tofIndex;
};

TofInfo tofInfo[4];

VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, tofPins[0]),
  VL53LX(&Wire, tofPins[1]),
  VL53LX(&Wire, tofPins[2]),
  VL53LX(&Wire, tofPins[3])
};

SemaphoreHandle_t xSemaphore;
#define MUTEX_TIMEOUT portMAX_DELAY


Tof tof[4];

int prevCount = -1;
uint32_t lastReading[4] = {0, 0, 0, 0};
TaskHandle_t xHandle[4] = {NULL, NULL, NULL, NULL};

// const static int tofPins[4] = {26, 25, 18, 19};

volatile TofData tofData;

void setup()
{
    Serial.begin(115200);
    delay(5000);

    Wire.begin();
    Wire.setClock(400000);

//     Serial.print("sensor Ptr: "); Serial.printf("%lu\n", (void*)&sensor_vl53lx_sat[0]);

     xSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(xSemaphore);



  for (int i = 0; i < 4; ++i){
    pinMode(tofPins[i], OUTPUT);
    digitalWrite(tofPins[i], LOW);
    tofInfo[i] = {&sensor_vl53lx_sat[i], {0, 0, 0, 0}, i};
  }

    Serial.println("Started");
    for (int i = 0; i < 4; i++){
        xSemaphoreTake(xSemaphore, MUTEX_TIMEOUT);
        xTaskCreate(
            tofTask, /* Task function. */
            "tof Task: ", /* name of task. */
            5000, /* Stack size of task */
            &tofInfo[i], /* parameter of the task */
            1, /* priority of the task */
            &xHandle[i]
        ); /* Task handle to keep track of created task */
        xSemaphoreGive(xSemaphore);
        delay(250);
    }
}

/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop()
{
    while(true){
        delay(1000);
        Serial.println("this is ESP32 Task");
        for (int i = 0; i < 4; ++i){
            if ((micros() - lastReading[i])/1000 > 500){

                Serial.printf("Watchdog go brrrrrrrr on %d\n", i);
                vTaskDelete(xHandle[i]);
                Serial.printf("We killed it %d\n", i);
                xSemaphoreGive(xSemaphore); // TODO: make this check if already taken

                xSemaphoreTake(xSemaphore, MUTEX_TIMEOUT);
                Serial.printf("about to create task after like 5 sec%d\n", i);
                delay(5000);
                Serial.printf("about to create task rn%d\n", i);

                xTaskCreate(
                    tofTask, /* Task function. */
                    "tof Task: ", /* name of task. */
                    5000, /* Stack size of task */
                    &tofInfo[i], /* parameter of the task */
                    1, /* priority of the task */
                    &xHandle[i]
                ); /* Task handle to keep track of created task */
                Serial.printf("made another task %d\n", i);
                xSemaphoreGive(xSemaphore);

            }
        }
    }
}

/* this function will be invoked when additionalTask was created */
void tofTask( void * tofInfoPtr )
{
    Wire.begin();

    TofInfo* currTofInfoPtr = static_cast<TofInfo*>(tofInfoPtr);
    xSemaphoreTake(xSemaphore, MUTEX_TIMEOUT);
    tof[currTofInfoPtr->tofIndex] = Tof(currTofInfoPtr->sensor, currTofInfoPtr->tofIndex);
    xSemaphoreGive(xSemaphore);
    Serial.println("Initialized");

    while(millis() < START_DELAY);
    delay(10);
    /* tof.print = false; */
    Serial.println("Started");
    /* loop forever */
    for(;;){
        lastReading[currTofInfoPtr->tofIndex] = micros();

        xSemaphoreTake(xSemaphore, MUTEX_TIMEOUT);
        Serial.printf("tof%d: poll: %lu\n", currTofInfoPtr->tofIndex, lastReading[currTofInfoPtr->tofIndex]);
        tof[currTofInfoPtr->tofIndex].poll();
        if (tof[currTofInfoPtr->tofIndex].getData().count != prevCount){
            Serial.printf("tof%d: Count: %d, Dist: %d, numObjs: %d, timeoutCount: %d\n",
            currTofInfoPtr->tofIndex, tof[currTofInfoPtr->tofIndex].getData().count, tof[currTofInfoPtr->tofIndex].getData().dist, tof[currTofInfoPtr->tofIndex].getData().numObjs, tof[currTofInfoPtr->tofIndex].getData().timeoutCount);
            prevCount = tof[currTofInfoPtr->tofIndex].getData().count;
        }
        xSemaphoreGive(xSemaphore);
        delay(5);
    }
    /* delete a task when finish,
    this will never happen because this is infinity loop */
    vTaskDelete( NULL );
}
