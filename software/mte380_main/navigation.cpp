#include "navigation.h"

void navigation_update_pos(sensor_data_t &sensor_data, Position &position){
  position.loc_x = 1;
  position.loc_y = 1;
  position.loc_z = 1;
  position.vel_x = 2;
  position.vel_y = 2;
  position.vel_z = 2;
  position.acc_x = 3;
  position.acc_y = 3;
  position.acc_z = 3;
  position.rot_xy = 4;
  position.rot_xz = 4;
  position.rot_yz = 4;
}
