#include "imu.h"
#include "motors.h"
#include "sensors.h"
#include "sorry.h"
#include "hms.h"
#include "hms_and_cmd_data.pb.h"

//creat TOF objects, not working when in tof.c
VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, tofPins[0]),
  VL53LX(&Wire, tofPins[1]),
  VL53LX(&Wire, tofPins[2]),
  VL53LX(&Wire, tofPins[3])
};

//create subsystem objects
Hms hms = Hms();
Motors motors = Motors(&hms);
Sensors sensors = Sensors(&hms, &sensor_vl53lx_sat[0], &motors);
Sorry sorry = Sorry(&motors, &sensors, &hms);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  delay(3000);

  Serial.println("Initializing Health Monitoring System");
  hms.init();
  Serial.println("Initializing Sensors");
  sensors.init();
  Serial.println("Done with init sensors");
  hms.data.mainLogLevel = HmsData_LogLevel_DEBUG;
  hms.data.sensorsLogLevel = HmsData_LogLevel_DEBUG;
}

void loop() {
  sorry.run();
}
