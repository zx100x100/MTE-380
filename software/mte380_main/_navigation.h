#ifndef NAVIGATION.H
#define NAVIGATION.H

#include <imuFilter.h>
#include "sensor_data.h"
#include "position.h"

class Navigation{
  private:
  position_t prev_position = {};
  float prev_certainty = 1.0;
  
  constexpr float gain = 0.1;
  imuFilter <&gain> imu_fusion;

  public:
  void initialize(sensor_data_t init_reading);

  void update_imu(sensor_data_t latest);

  position_t get_position();
  
}


#endif
