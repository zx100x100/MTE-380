/* #include "_old_navigation.h" */


/* class Navigation{ */
  /* void initialize(sensor_data_t init_reading){ */
    /* fusion.setup(init_reading.accel_x, init_reading.accel_y, init_reading.accel_z);   */
  /* } */

  /* void update_imu(sensor_data_t latest){ */
    /* fusion.update(latest.gyro_x, latest.gyro_y, latest.gyro_z, latest.accel_x, latest.accel_y, latest.accel_z); */

    /* float x[3], y[3], z[3]; */
    /* fusion.getXaxis( true, x ); */
    /* fusion.getYaxis( true, y ); */
    /* fusion.getZaxis( true, z ); */

    /* prev_position.x_loc = x[0]; */
    /* prev_position.y_loc = y[0]; */
    /* prev_position.z_loc = z[0]; */
    /* prev_position.x_velocity = x[1]; */
    /* prev_position.y_velocity = y[1]; */
    /* prev_position.z_velocity = z[1]; */
    /* prev_position.x_accel = x[2]; */
    /* prev_position.y_accel = y[2]; */
    /* prev_position.z_accel = z[2]; */

    /* prev_position.xy_rot = fusion.yaw(); */
    /* prev_position.xz_rot = fusion.pitch(); */
    /* prev_position.yz_rot = fusion.roll(); */
    /* prev_position.xy_ang_vel = latest.gyro_z; */
    /* prev_position.xz_ang_vel = latest.gyro_y; */
    /* prev_position.yz_ang_vel = latest.gyro_x; */
  /* } */

  /* position_t get_position(){ */
    /* return prev_position; */
  /* } */


/* } */
