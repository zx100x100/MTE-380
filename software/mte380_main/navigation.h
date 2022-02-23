#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "position.pb.h"
#include "sensor_data.h"

void navigation_update_pos(sensor_data_t &sensor_data, Position &position);

#endif
