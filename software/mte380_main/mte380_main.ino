#include "imu.h"
#include "motors.h"
#include "telemetry_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "hms.h"
#include "cmd_data.pb.h"

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
  sensors.update();
  nav.update();
  guidance.update();
  motors.update();
  telemetryServer.update();

  delay(400);
}
