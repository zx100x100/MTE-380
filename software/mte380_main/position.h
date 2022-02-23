#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>


typedef struct {
  union {
    float location[3];
    struct {
      float x_loc;
      float y_loc;
      float z_loc;
    };
  };
  
  union {
    float velocity[3];
    struct {
      float x_velocity;
      float y_velocity;
      float z_velocity;
    };
  };
  
  union {
    float accel[3];
    struct {
      float x_accel;
      float y_accel;
      float z_accel;
    };
  };
  
  union {
    float rotation[3];
    struct {
      float xy_rot;  // will not be modulus-ed
      float xz_rot;
      float yz_rot;
    };
  };
  
  union {
    float angular_vel[3];
    struct {
      float xy_ang_vel;
      float xz_ang_vel;
      float yz_ang_vel;
    };
  };
} position_t;
#endif
