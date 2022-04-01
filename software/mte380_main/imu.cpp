#include <Wire.h>

#include "imu.h"

Imu::Imu(){
}

// TODO do we need this to return a bool (success/fail?)
Imu::Imu(Hms* hms):
  hms(hms)
{
  delay(500);
  imuData = ImuData_init_zero;
  // return;
  bool success = true;
  //ACCELL
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 accell detected ... Check your wiring!");
    success = false;
  }
  //MAG
  if(!mag.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 mag detected ... Check your wiring!");
    success = false;
  }
  //GYRO
  /* gyro.enableAutoRange(true); */
  if(!gyro.begin(GYRO_RANGE_2000DPS))
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
    success = false;
  }
  delay(200);

//  displayDetails();
  /* return success; */
}

void Imu::poll(){
  // imuData.timestamp = micros();
  sensors_event_t event;
  
  accel.getEvent(&event);
  imuData.accelX = event.acceleration.x;
  imuData.accelY = event.acceleration.y;
  imuData.accelZ = event.acceleration.z;
  

  mag.getEvent(&event);
  imuData.magX = event.magnetic.x;
  imuData.magY = event.magnetic.y;
  imuData.magZ = event.magnetic.z;
  
  gyro.getEvent(&event);
  imuData.gyroX = event.gyro.x;
  imuData.gyroY = event.gyro.y;
  imuData.gyroZ = event.gyro.z;
//  Serial.print("Gyro: x: "); Serial.print(imuData.gyroX); Serial.print(", y: "); Serial.print(imuData.gyroY); Serial.print(", z: "); Serial.println(imuData.gyroZ);
  // Serial.print("imuData ptr:");
  // Serial.print((unsigned long)&imuData);
  // Serial.print("imuData.accelZ:");
  // Serial.println(imuData.accelZ);
}

ImuData& Imu::getData(){
  return imuData;
}

//void Imu::displayDetails(){
//  //ACCEL
//  sensor_t sensor;
//  accel.getSensor(&sensor);
//  Serial.println("------------------------------------");
//  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
//  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
//  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
//  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
//  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
//  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
//  Serial.println("------------------------------------");
//  Serial.println("");
//
//  //MAG
//  mag.getSensor(&sensor);
//  Serial.println("------------------------------------");
//  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
//  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
//  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
//  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
//  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
//  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
//  Serial.println("------------------------------------");
//  Serial.println("");
//
//  //GYRO
//  gyro.getSensor(&sensor);
//  Serial.println("------------------------------------");
//  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
//  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
//  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
//  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" rad/s");
//  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" rad/s");
//  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" rad/s");
//  Serial.println("------------------------------------");
//  Serial.println("");
//}
