#include "time_of_flight.h"

#include <Wire.h>


void TOF_select(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}


bool TOF_init(void)
{
  bool success = true;

  return success;
}

void poll_TOFs(sensor_data_t &sensor_data)
{
  
  //sensor_data.accel_x = event.acceleration.x;
}
