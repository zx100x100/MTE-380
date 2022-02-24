#include "imu.h"
#include "motors.h"
#include "tcp_server.h"
#include "nav.h"
#include "guidance.h"
#include "sensors.h"
#include "cmd_data.pb.h"

#define LEFT_DRIVE_PIN 2
#define RIGHT_DRIVE_PIN 4

//subsystems
Sensors sensors;
Nav nav(sensors);
CmdData cmdData = CmdData_init_zero;
Guidance guidance(nav.getData(), cmdData);
TcpServer tcpServer(sensors, nav.getData(), guidance.getData(), cmdData);
Motors motors(guidance.getData(), LEFT_DRIVE_PIN, RIGHT_DRIVE_PIN);

void setup() {
  Serial.begin(115200);

  // sensors.init();
  nav.init();
}

void loop() {
  sensors.update();
  nav.update();
  guidance.update();
  motors.update();
  tcpServer.update();
}
