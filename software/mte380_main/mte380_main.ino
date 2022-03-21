#include "imu.h"
#include "motors.h"
#include "telemetry_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "hms.h"
#include "hms_and_cmd_data.pb.h"

VL53LX sensor_vl53lx_sat[4] = {
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN),
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN),
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN),
  VL53LX(&Wire, TOF_PLACEHOLDER_PIN)
};

unsigned long longest = 0;
//subsystems
Hms hms = Hms();
CmdData cmdData = CmdData_init_zero;
Sensors sensors = Sensors(&hms, &sensor_vl53lx_sat[0]);
Nav nav = Nav(sensors, &cmdData, &hms);
Guidance guidance = Guidance(nav.getData(), cmdData, &hms);
TelemetryServer telemetryServer = TelemetryServer(sensors,
                                                  nav.getData(),
                                                  guidance.getData(),
                                                  cmdData,
                                                  &hms);
Motors motors = Motors(guidance.getData(), &hms);

void setup() {
  Serial.begin(115200);
  pinMode(15, INPUT);
  hms.init();

  /* cmdData.telemetryMode = CmdData_TelemetryMode_NONE; */
  /* cmdData.telemetryMode = CmdData_TelemetryMode_FULL; */
  cmdData.runState = CmdData_RunState_E_STOP;
  sensors.init();
  nav.init();
  telemetryServer.init();
  /* guidance.init(); */
}

void loop() {
  unsigned long startT = micros();
  Serial.println("main->sensors");
  sensors.update();
  unsigned long afterSensorT = micros();
  Serial.println("main->nav");
  nav.update();
  Serial.println("main->guidance");
  guidance.update();
  Serial.println("main->motors");
  motors.update();
  Serial.println("main->telemetry");
  unsigned long beforeNetworkT = micros();
  bool updated = telemetryServer.update();
  unsigned long afterNetworkT = micros();
  hms.data.mainTickRate = beforeNetworkT - startT;
  hms.data.networkTickRate = afterNetworkT - beforeNetworkT;
  hms.data.combinedTickRate = afterNetworkT - startT;
  hms.data.sensorsTickRate = afterSensorT - startT;
  if (hms.data.networkTickRate > longest){
    longest = hms.data.networkTickRate;
    hms.data.longestCombinedTick = longest;
  }
  hms.update();

  /* delay(500); */
}
