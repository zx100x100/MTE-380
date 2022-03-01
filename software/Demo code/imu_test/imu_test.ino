#include "sensor_data.h"
#include "nav.h"


sensor_data_t sensor_data;
Nav nav;

void setup() {
  Serial.begin(115200);
  sensor_init();

  while(micros() < 200000){}  

  float avg_accel[3] = {0, 0, 0};
  float avg_gyro[3] = {0, 0, 0};
  for (int i = 0; i < 10; ++i){
    sensor_poll(sensor_data);
    Serial.print("Accel:");
    Serial.print("X: "); Serial.print(sensor_data.accel_x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(sensor_data.accel_y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(sensor_data.accel_z); Serial.print("  ");Serial.print("m/s^2 ; ");
  
    Serial.print("Mag:");
    Serial.print("X: "); Serial.print(sensor_data.mag_x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(sensor_data.mag_y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(sensor_data.mag_z); Serial.print("  ");Serial.print("uT ; ");
  
    Serial.print("Gyro:");
    Serial.print("X: "); Serial.print(sensor_data.gyro_x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(sensor_data.gyro_y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(sensor_data.gyro_z); Serial.print("  ");Serial.println("rad/s");
    for (int j = 0; j < 3; ++j){
      avg_accel[j] += sensor_data.accel_v[j];
      avg_gyro[j] += sensor_data.gyro_v[j];
    }
  }
  for (int i = 0; i < 3; ++i){
      avg_accel[i] /= 10;
      avg_gyro[i] /= 10;
  }
  nav.init(avg_accel, avg_gyro);

}

void loop() {
  sensor_poll(sensor_data);
  nav.update(sensor_data);
  position_t pos = nav.get_position();
  
  
  //get_sensor_data()
  //pos_hb(sensor_data)
  //control_hb(position)
  //drive_hb(control.commands)

  Serial.print("Time: ");
  Serial.println(sensor_data.timestamp);

  Serial.print("X: "); Serial.print(pos.x_loc); Serial.print(" ");
  Serial.print("Y: "); Serial.print(pos.y_loc); Serial.print(" ");
  Serial.print("Z: "); Serial.print(pos.z_loc); Serial.print(" m; ");
  Serial.print("X: "); Serial.print(pos.x_velocity); Serial.print(" ");
  Serial.print("Y: "); Serial.print(pos.y_velocity); Serial.print(" ");
  Serial.print("Z: "); Serial.print(pos.z_velocity); Serial.print(" m/s; ");
  Serial.print("X: "); Serial.print(pos.x_accel); Serial.print(" ");
  Serial.print("Y: "); Serial.print(pos.y_accel); Serial.print(" ");
  Serial.print("Z: "); Serial.print(pos.z_accel); Serial.print(" m/s^2; ");
  Serial.print("yaw: "); Serial.print(pos.xy_rot); Serial.print(" ");
  Serial.print("roll: "); Serial.print(pos.xz_rot); Serial.print(" ");
  Serial.print("pitch: "); Serial.print(pos.yz_rot); Serial.print(" ; "); Serial.println();

//  Serial.print("Accel:");
//  Serial.print("X: "); Serial.print(sensor_data.accel_x); Serial.print("  ");
//  Serial.print("Y: "); Serial.print(sensor_data.accel_y); Serial.print("  ");
//  Serial.print("Z: "); Serial.print(sensor_data.accel_z); Serial.print("  ");Serial.print("m/s^2 ; ");
//
//  Serial.print("Mag:");
//  Serial.print("X: "); Serial.print(sensor_data.mag_x); Serial.print("  ");
//  Serial.print("Y: "); Serial.print(sensor_data.mag_y); Serial.print("  ");
//  Serial.print("Z: "); Serial.print(sensor_data.mag_z); Serial.print("  ");Serial.print("uT ; ");
//
//  Serial.print("Gyro:");
//  Serial.print("X: "); Serial.print(sensor_data.gyro_x); Serial.print("  ");
//  Serial.print("Y: "); Serial.print(sensor_data.gyro_y); Serial.print("  ");
//  Serial.print("Z: "); Serial.print(sensor_data.gyro_z); Serial.print("  ");Serial.println("rad/s");


}
