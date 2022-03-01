#ifndef NAV_H
#define NAV_H

#include <imuFilter.h>
#include "sensor_data.h"
#include "position.h"

class Nav{
  public:
    Nav();
    void init(float init_accel[], float init_gyro[]);
    void update(sensor_data_t latest);
    position_t get_position();

  private:
    constexpr static float gain = 0.1;
    imuFilter <&gain> imu_fusion;
    position_t prev_position = {};

    const static bool to_world = true;
    #define ARR_SIZE 3
    float gyro_bias[ARR_SIZE];
    float accel_bias[ARR_SIZE];
    float avg_accel[ARR_SIZE] = {0, 0, 0};
    int num_readings = 0;

};

#endif
