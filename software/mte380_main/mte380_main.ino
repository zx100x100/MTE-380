#include "sensor_data.h"
#include "drive.h"
<<<<<<< HEAD
#include "wifi_server.h"
#include "position.pb.h"
#include "navigation.h"
#include "command.pb.h"
=======
/* #include "bluetooth.h" */
#include "wifi_server.h"
>>>>>>> 1b02b63bb6d6ec40731a0fc67adaf2662be04a12

sensor_data_t sensor_data;
drive_command_t drive_command;
Position position = Position_init_zero;
Command command = Command_init_zero; // this should entirely replace drive_command_t later

void setup() {
  Serial.begin(115200);
  sensor_init();
  drive_init(2, 4);
<<<<<<< HEAD
=======
  /* bluetooth_setup(); */
>>>>>>> 1b02b63bb6d6ec40731a0fc67adaf2662be04a12
  wifi_init();
  host_server();
}

void loop() {
  sensor_poll(sensor_data);
  //get_sensor_data()

  // TODO maybe have an array of timestamped positions
  // TODO and an array of timestamped sensor data
  // for Ahmad to do spicy algorithms on??
  navigation_update_pos(sensor_data, position);
  /* pos_hb(sensor_data) */
  //control_hb(position)
  drive_command.left_power = 0;//255;
  drive_command.right_power = 0;//255;
  set_drive(drive_command);

  server_tick(command, position);

  delay(500);
                  
  /*Serial.print("Time: ");
  Serial.println(sensor_data->timestamp);

  Serial.print("Accel:");
  Serial.print("X: "); Serial.print(sensor_data.accel_x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(sensor_data.accel_y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(sensor_data.accel_z); Serial.print("  ");Serial.println("m/s^2 ");

  Serial.print("Mag:");
  Serial.print("X: "); Serial.print(sensor_data.mag_x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(sensor_data.mag_y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(sensor_data.mag_z); Serial.print("  ");Serial.println("uT");

  Serial.print("Gyro:");
  Serial.print("X: "); Serial.print(sensor_data.gyro_x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(sensor_data.gyro_y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(sensor_data.gyro_z); Serial.print("  ");Serial.println("rad/s");*/
}
