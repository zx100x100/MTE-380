#include "sensor_data.h"
#include "drive.h"
/* #include "bluetooth.h" */
#include "wifi_server.h"

sensor_data_t sensor_data;
drive_command_t drive_command;

void setup() {
  Serial.begin(115200);
  sensor_init();
  drive_init(2, 4);
  /* bluetooth_setup(); */
  wifi_init();
  host_server();
}

void loop() {
  sensor_poll(sensor_data);
  //get_sensor_data()
  //pos_hb(sensor_data)
  //control_hb(position)
  drive_command.left_power = 0;//255;
  drive_command.right_power = 0;//255;
  set_drive(drive_command);
                  
  /* Serial.print("Time: "); */
  /* Serial.println(sensor_data.timestamp); */

  /* Serial.print("Accel:"); */
  /* Serial.print("X: "); Serial.print(sensor_data.accel_x); Serial.print("  "); */
  /* Serial.print("Y: "); Serial.print(sensor_data.accel_y); Serial.print("  "); */
  /* Serial.print("Z: "); Serial.print(sensor_data.accel_z); Serial.print("  ");Serial.println("m/s^2 "); */

  /* Serial.print("Mag:"); */
  /* Serial.print("X: "); Serial.print(sensor_data.mag_x); Serial.print("  "); */
  /* Serial.print("Y: "); Serial.print(sensor_data.mag_y); Serial.print("  "); */
  /* Serial.print("Z: "); Serial.print(sensor_data.mag_z); Serial.print("  ");Serial.println("uT"); */

  /* Serial.print("Gyro:"); */
  /* Serial.print("X: "); Serial.print(sensor_data.gyro_x); Serial.print("  "); */
  /* Serial.print("Y: "); Serial.print(sensor_data.gyro_y); Serial.print("  "); */
  /* Serial.print("Z: "); Serial.print(sensor_data.gyro_z); Serial.print("  ");Serial.println("rad/s"); */

  server_tick();
  /* bool connected = bluetooth_loop(millis()); */
  /* Serial.print("BT: "); Serial.print(connected); Serial.println("  "); */
}
