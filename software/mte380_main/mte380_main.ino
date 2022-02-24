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

  // sensors.init();
  nav.init();
  telemetryServer.init();
}

void loop() {
  int ts1 = millis();
  sensors.update();
  nav.update();
  guidance.update();
  motors.update();
  int ts2 = millis();
  telemetryServer.update();
  int ts3 = millis();
  /* Serial.print("ts2-ts1="); Serial.print(ts2-ts1); Serial.print(","); */
  /* Serial.print("ts3-ts2="); Serial.print(ts3-ts2); Serial.print(","); */
  /* Serial.print("ts3-ts1="); Serial.print(ts3-ts1); Serial.println(""); */
}
