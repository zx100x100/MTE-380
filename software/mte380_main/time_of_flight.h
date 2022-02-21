#ifndef TIME_OF_FLIGHT_H
#define TIME_OF_FLIGHT_H

#include <stdint.h>
#include "sensor_data.h"

#define TCAADDR 0x70

void TOF_select(uint8_t i);

bool TOF_init(void);

void poll_TOFs(sensor_data_t &sensor_data);


#endif
