#include "imu.h"
#include "motors.h"
#include "telemetry_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "hms.h"
#include "hms_and_cmd_data.pb.h"

/* #define RUN_TURN_IN_PLACE_TEST */

//creat TOF objects, not working when in tof.c
VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN),
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN),
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN),
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN)
};

//create subsystem objects
Hms hms = Hms();
CmdData cmdData = CmdData_init_zero;
Sensors sensors = Sensors(&hms, &sensor_vl53lx_sat[0]);
Nav nav = Nav(sensors, &cmdData, &hms);
Guidance guidance = Guidance(nav.getData(),
                             cmdData,
                             &hms,
                             NULL,
                             &nav);
TelemetryServer telemetryServer = TelemetryServer(sensors,
                                                  nav.getData(),
                                                  guidance.getData(),
                                                  cmdData,
                                                  &hms);
Motors motors = Motors(guidance.getData(), &hms);



void setup() {
  Serial.begin(115200);

  // this delay is to be able to put the robot on the ground before it starts measuring. SPEED!!!!
  delay(3000);

  hms.init();
  sensors.init();
  nav.init();
  guidance.init();

  guidance.motors = &motors;
  cmdData.runState = CmdData_RunState_E_STOP;
  
#ifdef RUN_TURN_IN_PLACE_TEST
  delay(1000);
  guidance.turnInPlace();
  while(true){}
#endif
#ifndef RUN_TURN_IN_PLACE_TEST
  telemetryServer.init();
#endif
}

void loop() {
  unsigned long startT = micros();

  delay(8); // Confirm uneeded for sensors and delete!
  
  // Sensors
  if (hms.data.mainLogLevel >= 1){
    Serial.println("main->sensors");
  }
  sensors.update();
  unsigned long afterSensorT = micros();
  
  // Guidence
  if (hms.data.mainLogLevel >= 1){
    Serial.println("main->guidance");
  }
  guidance.update();
  
  // Motors
  if (hms.data.mainLogLevel >= 1){
    Serial.println("main->motors");
  }
  motors.update();
  
  // Telemetry
  if (hms.data.mainLogLevel >= 1){
    Serial.println("main->telemetry");
  }
  unsigned long beforeNetworkT = micros();
  bool updated = telemetryServer.update();
  unsigned long afterNetworkT = micros();
  // TODO make hms heartbeat function:
  hms.data.mainTickRate = beforeNetworkT - startT;
  hms.data.networkTickRate = afterNetworkT - beforeNetworkT;
  hms.data.combinedTickRate = afterNetworkT - startT;
  hms.data.sensorsTickRate = afterSensorT - startT;
  hms.update();
}
