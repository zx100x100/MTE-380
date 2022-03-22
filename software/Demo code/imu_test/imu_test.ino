#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>

#include <imuFilter.h>

/* Assign a unique ID to the sensors */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);


constexpr float GAIN = 0.1;     // Fusion gain, value between 0 and 1 - Determines response of heading correction with respect to gravity.
imuFilter <&GAIN> fusion;

// #define NUM_TO_AVG 100
// float gyroAvg[3] = { 0, 0, 0 };

void setup(void)
{
  delay(2000);
  Serial.begin(115200);
  Serial.println(F("Adafruit 10DOF Tester")); Serial.println("");

  /* Initialise the sensors */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.print("Ooops, no L3GD20 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

//   for (int i = 0; i < NUM_TO_AVG; ++i){
//     sensors_event_t event;
//     gyro.getEvent(&event);
//     gyroAvg[0] += event.gyro.x;
//     gyroAvg[1] += event.gyro.y;
//     gyroAvg[2] += event.gyro.z;
//   }
//   for (int i = 0; i < 3; ++i){
//     gyroAvg[i] /= float(NUM_TO_AVG);
//   }

  sensors_event_t event;
  /* Display the results (acceleration is measured in m/s^2) */
  accel.getEvent(&event);
  // Initialize filter:
  fusion.setup( event.acceleration.x, event.acceleration.y, event.acceleration.z );

}

void loop(void)
{
  /* Get a new sensor event */
  uint32_t start = micros();
  sensors_event_t accEvent;
  sensors_event_t gyroEvent;

  accel.getEvent(&accEvent);
  gyro.getEvent(&gyroEvent);


//   Serial.print(F("GYRO  "));
//   Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");
//   Serial.print("Y: "); Serial.print(event.gyro.y); Serial.print("  ");
//   Serial.print("Z: "); Serial.print(event.gyro.z); Serial.print("  ");Serial.println("rad/s ");


  // Update filter:
//   float gyroX = gyroEvent.gyro.x - gyroAvg[0];
//   float gyroY = gyroEvent.gyro.y - gyroAvg[1];
//   float gyroZ = gyroEvent.gyro.z - gyroAvg[2];

  fusion.update( gyroEvent.gyro.x, gyroEvent.gyro.y, gyroEvent.gyro.z, accEvent.acceleration.x, accEvent.acceleration.y, accEvent.acceleration.z );

  uint32_t del = micros() - start;

  // Display angles:
//   Serial.print( del);
//   Serial.print( " " );
//   Serial.print( accEvent.acceleration.x);
//   Serial.print( " " );
//   Serial.print( accEvent.acceleration.y);
//   Serial.print( " " );
//   Serial.print( accEvent.acceleration.z);
//   Serial.print( " " );
//   Serial.print( gyroEvent.gyro.x);
//   Serial.print( " " );
//   Serial.print( gyroEvent.gyro.y);
//   Serial.print( " " );
//   Serial.print( gyroEvent.gyro.z);
//   Serial.print( " " );
//   Serial.print( fusion.pitch() *180/PI);
//   Serial.print( " " );
//   Serial.print( fusion.roll() *180/PI);
//   Serial.print( " " );
  Serial.print( fusion.yaw() *180/PI);
  Serial.println( " " );
}