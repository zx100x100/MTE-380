#include "imu.h"
#include "motors.h"
#include "telemetry_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "sorry.h"
#include "hms.h"
#include "hms_and_cmd_data.pb.h"

/* #define RUN_TURN_IN_PLACE_TEST */
/* #define NO_SENSORS */
/* #define NO_NAV */
/* #define NO_TELEMETRY_RUN_AUTO */
#define SORRY

//creat TOF objects, not working when in tof.c
VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, tofPins[0]),
  VL53LX(&Wire, tofPins[1]),
  VL53LX(&Wire, tofPins[2]),
  VL53LX(&Wire, tofPins[3])
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
                             &nav,
                             &sensors);
#ifndef SORRY
TelemetryServer telemetryServer = TelemetryServer(sensors,
                                                  nav.getData(),
                                                  guidance.getData(),
                                                  cmdData,
                                                  &hms);
#endif
Motors motors = Motors(guidance.getData(), &hms);

#ifdef SORRY
Sorry sorry = Sorry(&motors, &sensors, &nav, &hms);
#endif



void setup() {
  Serial.begin(115200);

  // this delay is to be able to put the robot on the ground before it starts measuring. SPEED!!!!
  delay(5000);

  Serial.println("Initializing Health Monitoring System");
  hms.init();
#ifndef NO_SENSORS
  Serial.println("Initializing Sensors");
  sensors.init();
#endif
  Serial.println("Initializing Navigation");
#ifndef NO_NAV
  nav.init();
#endif
  Serial.println("Initializing Guidance");
  guidance.init();

  guidance.motors = &motors;
  cmdData.runState = CmdData_RunState_E_STOP;

#ifdef NO_TELEMETRY_RUN_AUTO
  cmdData.runState = CmdData_RunState_AUTO;
  /* hms.data.mainLogLevel = HmsData_LogLevel_OVERKILL; */
  hms.data.mainLogLevel = HmsData_LogLevel_NORMAL;

  hms.data.sensorsLogLevel = HmsData_LogLevel_NORMAL;

  /* hms.data.navLogLevel = HmsData_LogLevel_DEBUG; */
  hms.data.navLogLevel = HmsData_LogLevel_DEBUG;

  /* hms.data.guidanceLogLevel = HmsData_LogLevel_OVERKILL; */
  /* hms.data.guidanceLogLevel = HmsData_LogLevel_NORMAL; */
  hms.data.guidanceLogLevel = HmsData_LogLevel_DEBUG;

  guidance.gd.kP_vel = 130.0;//140.0;
  guidance.gd.kI_vel = 40.0;
  guidance.gd.kD_vel = 18000;
  /* guidance.gd.kP_drift = 130.0; */
  /* guidance.gd.kI_drift = 0; */
  /* guidance.gd.kD_drift = 40; */
  guidance.gd.kP_drift = 1.1;
  guidance.gd.kD_drift = 2500;
  guidance.gd.kI_drift = 0.02 * (hms.data.nCells < 3 ? 1.5: 1);
#endif
  
#ifdef RUN_TURN_IN_PLACE_TEST
  delay(1000);
  guidance.turnInPlace();
  while(true){}
#endif
#ifndef NO_TELEMETRY_RUN_AUTO
#ifndef SORRY
  Serial.println("Initializing Telemetry");
  telemetryServer.init();
#endif
#endif
}

void loop() {
#ifdef SORRY
  sorry.run();
  while(true){}
#endif
  unsigned long startT = micros();

  // Sensors
  if (hms.data.mainLogLevel >= 1){
    Serial.println("main->sensors");
  }
#ifndef NO_SENSORS
  sensors.update();
#endif
  unsigned long afterSensorT = micros();
  
  // Guidance
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
unsigned long beforeNetworkT = micros();
#ifndef NO_TELEMETRY_RUN_AUTO
#ifndef SORRY
  if (hms.data.mainLogLevel >= 1){
    Serial.println("main->telemetry");
  }
  bool updated = telemetryServer.update();
#endif
#endif
  unsigned long afterNetworkT = micros();
  // TODO make hms heartbeat function:
  hms.data.mainTickRate = beforeNetworkT - startT;
  hms.data.networkTickRate = afterNetworkT - beforeNetworkT;
  hms.data.combinedTickRate = afterNetworkT - startT;
  hms.data.sensorsTickRate = afterSensorT - startT;
  hms.update();

  /* uint8_t xNum = uint8_t('x'); */
  /* char xChar = char(xNum); */
  /* Serial.print("xNum: "); Serial.println(xNum); // should be 120 */
  /* Serial.print("xChar: "); Serial.println(xChar); // should be x */
  /* while(true){} */
}
