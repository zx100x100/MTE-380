#include "fusion.h"
#include "nav.h"

Nav::Nav(SensorData &sensorData)
  :sensorData(sensorData){
  navData = NavData_init_zero;
  fusion = Fusion();
  /* fusion.setup(); */

  // TODO use sensorData instead of init_reading
  /* fusion.setup(init_reading.accel_x, init_reading.accel_y, init_reading.accel_z);   */
}

void Nav::init(){
  fusion.setup(sensorData.imuData.accelX, sensorData.imuData.accelY, sensorData.imuData.accelZ);
}

void Nav::updateImu(){
  ImuData &latest = sensorData.imuData;
  // fusion.update(latest.gyroX, latest.gyroY, latest.gyroZ, latest.accelX, latest.accelY, latest.accelZ);
  // float x[3], y[3], z[3];
  // fusion.getXaxis( true, x );
  // fusion.getYaxis( true, y );
  // fusion.getZaxis( true, z );

  // prev_position.x_loc = x[0];
  // prev_position.y_loc = y[0];
  // prev_position.z_loc = z[0];
  // prev_position.x_velocity = x[1];
  // prev_position.y_velocity = y[1];
  // prev_position.z_velocity = z[1];
  // prev_position.x_accel = x[2];
  // prev_position.y_accel = y[2];
  // prev_position.z_accel = z[2];

  // prev_position.xy_rot = fusion.yaw();
  // prev_position.xz_rot = fusion.pitch();
  // prev_position.yz_rot = fusion.roll();
  // prev_position.xy_ang_vel = latest.gyro_z;
  // prev_position.xz_ang_vel = latest.gyro_y;
  // prev_position.yz_ang_vel = latest.gyro_x;
}

void Nav::update(){
  navData.posX = 1;
  navData.posY = 1;
  navData.posZ = 1;
  navData.velX = 2;
  navData.velY = 2;
  navData.velZ = 2;
  navData.accX = 3;
  navData.accY = 3;
  navData.accZ = 3;
  navData.angXy = 4;
  navData.angXz = 4;
  navData.angYz = 4;
  navData.angVelXy = 5;
  navData.angVelXz = 5;
  navData.angVelYz = 5;
}

NavData& Nav::getData(){
  return navData;
}
