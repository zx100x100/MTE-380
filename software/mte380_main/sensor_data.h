#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>

typedef struct {
  uint32_t timestamp;
  
  union {
    float accel_v[3];
    struct {
      float accel_x;
      float accel_y;
      float accel_z;
    };
  };

  union {
    float mag_v[3];
    struct {
      float mag_x;
      float mag_y;
      float mag_z;
    };
  };

  union {
    float gyro_v[3];
    struct {
      float gyro_x;
      float gyro_y;
      float gyro_z;
    };
  };
} sensor_data_t;


bool sensor_init(void);

void sensor_poll(sensor_data_t &sensor_data);

void display_sensor_details(void);


#endif
