#include "imu.h"
#include "motors.h"
#include "telemetry_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "hms.h"
#include "hms_and_cmd_data.pb.h"

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
  guidance.init();
}

void loop() {
  unsigned long startT = micros();
  sensors.update();
  nav.update();
  guidance.update();
  motors.update();
  unsigned long beforeNetworkT = micros();
  bool updated = telemetryServer.update();
  unsigned long afterNetworkT = micros();
  hms.data.mainTickRate = beforeNetworkT - startT;
  hms.data.networkTickRate = afterNetworkT - beforeNetworkT;
  hms.data.combinedTickRate = afterNetworkT - startT;
  if (hms.data.networkTickRate > longest){
    longest = hms.data.networkTickRate;
    hms.data.longestCombinedTick = longest;
  }

  /* delay(400); */
}
