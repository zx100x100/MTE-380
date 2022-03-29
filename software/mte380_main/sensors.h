#ifndef SENSORS_H
#define SENSORS_H

#include "hms.h"
#include "tof.h"
#include "imu.h"


#include "src/VL53L3CX_lib/vl53lx_class.h"

enum TofOrder {FRONT, L_FRONT, L_BACK, BACK};


class Sensors{
  public:
    /*Sensors(); */
     Sensors(Hms* hms, VL53LX tof_objects[]);
     bool init();
     void initGyro();
    // Sensors& getData();
    void update(bool skipTOFs=false, bool skipJustBackTof=false);
    Imu imu;
    Tof tof[4];
    long timestamp;
    Hms* hms;
  private:
    // TODO update IMU and TOFs
    void updateBatteryVoltage();
    VL53LX* sensor_vl53lx_sat[4];
};

#endif
