#include "nav.h"

Nav::Nav(){
  for (int i = 0; i < ARR_SIZE; ++i){
    prev_position.location[i] = 0;
    prev_position.velocity[i] = 0;
    prev_position.accel[i] = 0;
    prev_position.rotation[i] = 0;
    prev_position.angular_vel[i] = 0;
  }
  prev_position.timestamp = 0;
}

void Nav::init(float init_accel[], float init_gyro[])
{
    imu_fusion.setup(init_accel[0], init_accel[1], init_accel[2]);

    for (int i = 0; i < ARR_SIZE; ++i){
      gyro_bias[i] = init_gyro[i];
      accel_bias[i] = init_accel[i];
    }
    Serial.print("accel bias: "); Serial.print(accel_bias[0]); Serial.print("; "); Serial.print(accel_bias[1]); Serial.print("; ");  Serial.print(accel_bias[2]);
    Serial.print("; gyro bias: "); Serial.print(gyro_bias[0]); Serial.print("; "); Serial.print(gyro_bias[1]); Serial.print("; ");  Serial.println(gyro_bias[2]);
}

void Nav::update(sensor_data_t latest)
{
  float del_t = (latest.timestamp - prev_position.timestamp) / 1000000.0;
  
  float gyro[ARR_SIZE];
  for (int i = 0; i < ARR_SIZE; ++i){
    gyro[i] = latest.gyro_v[i] - gyro_bias[i];
  }
  float ax_local = latest.accel_x, ay_local = latest.accel_y, az_local = latest.accel_z;
  
  imu_fusion.update(gyro[0], gyro[1], gyro[2], ax_local, ay_local, az_local);

  float x[3], y[3], z[3];
  imu_fusion.getXaxis( to_world, x );
  imu_fusion.getYaxis( to_world, y );
  imu_fusion.getZaxis( to_world, z );

  float ax_global = ax_local*x[0] + ay_local*y[0] + az_local*z[0];
  float ay_global = ax_local*x[1] + ay_local*y[1] + az_local*z[1];
  float az_global = ax_local*x[2] + ay_local*y[2] + az_local*z[2] - accel_bias[2];

  avg_accel[0] = (avg_accel[0] * num_readings + ax_global) / (num_readings + 1);
  avg_accel[1] = (avg_accel[1] * num_readings + ay_global) / (num_readings + 1);
  avg_accel[2] = (avg_accel[2] * num_readings + az_global) / (num_readings + 1);
  num_readings += 1;

  Serial.print("x: "); Serial.print(x[0]); Serial.print("; "); Serial.print(x[1]); Serial.print("; ");  Serial.print(x[2]);
  Serial.print("; y: "); Serial.print(y[0]); Serial.print("; "); Serial.print(y[1]); Serial.print("; ");  Serial.print(y[2]);
  Serial.print("; z: "); Serial.print(z[0]); Serial.print("; "); Serial.print(z[1]); Serial.print("; ");  Serial.print(z[2]); Serial.print("; del_t: "); Serial.println(del_t);
  Serial.print("ax_local: "); Serial.print(ax_local); Serial.print(";ay_local: "); Serial.print(ay_local); Serial.print(";az_local: "); Serial.println(az_local); 
  Serial.print("ax_global: "); Serial.print(ax_global); Serial.print(";ay_global: "); Serial.print(ay_global); Serial.print(";az_global: "); Serial.println(az_global); 
  Serial.print("avg_accel: "); Serial.print(avg_accel[0]); Serial.print("; "); Serial.print(avg_accel[1]); Serial.print("; ");  Serial.println(avg_accel[2]);

  prev_position.x_loc += ax_global / 2 * del_t*del_t;
  prev_position.y_loc += ay_global / 2 * del_t*del_t;
  prev_position.z_loc += az_global / 2 * del_t*del_t;

  prev_position.x_velocity += ax_global * del_t;
  prev_position.y_velocity += ay_global * del_t;
  prev_position.z_velocity += az_global * del_t;

  prev_position.x_accel = ax_global;
  prev_position.y_accel = ay_global;
  prev_position.z_accel = az_global;

  prev_position.xy_rot = imu_fusion.yaw();
  prev_position.xz_rot = imu_fusion.pitch();
  prev_position.yz_rot = imu_fusion.roll();
  prev_position.xy_ang_vel = latest.gyro_z;
  prev_position.xz_ang_vel = latest.gyro_y;
  prev_position.yz_ang_vel = latest.gyro_x;

  prev_position.timestamp = latest.timestamp;
}

position_t Nav::get_position(){
  return prev_position;
}
