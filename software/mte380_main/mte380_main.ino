#include "imu.h"
#include "motors.h"
#include "telemetry_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "hms.h"
#include "cmd_data.pb.h"

unsigned long longest = 0;
//subsystems
Hms hms = Hms();
CmdData cmdData = CmdData_init_zero;
Sensors sensors = Sensors(&hms);
Nav nav = Nav(sensors, &hms);
Guidance guidance = Guidance(nav.getData(), cmdData, &hms);
TelemetryServer telemetryServer = TelemetryServer(sensors,
                                                  nav.getData(),
                                                  guidance.getData(),
                                                  cmdData,
                                                  &hms);
Motors motors = Motors(guidance.getData(), &hms);

void setup() {
  Serial.begin(115200);

  cmdData.runState = CmdData_RunState_E_STOP;
  nav.init();
  telemetryServer.init();
}

void loop() {
  unsigned long startT = micros();
  if (hms.data.logLevel >= 2) Serial.println("sensors");
  if (hms.data.logLevel >= 2) sensors.update();
  if (hms.data.logLevel >= 2) Serial.println("nav");
  nav.update();
  if (hms.data.logLevel >= 2) Serial.println("guidance");
  guidance.update();
  if (hms.data.logLevel >= 2) Serial.println("motors");
  motors.update();
  if (hms.data.logLevel >= 2) Serial.println("telemetry");
  unsigned long beforeNetworkT = micros();
  bool updated = telemetryServer.update();
  unsigned long afterNetworkT = micros();
  hms.data.mainTickRate = beforeNetworkT - startT;
  if (updated){
    hms.data.networkTickRate =  afterNetworkT - beforeNetworkT;
  }
  hms.data.combinedTickRate = afterNetworkT - startT;
  if (hms.data.combinedTickRate > longest){
    hms.data.longestCombinedTick = hms.data.combinedTickRate;
    longest = hms.data.combinedTickRate;
  }

  /* delay(400); */
}
