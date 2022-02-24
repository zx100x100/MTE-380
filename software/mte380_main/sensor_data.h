#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "imu_data.pb.h"
#include "tof_data.pb.h"

class SensorData{
  public:
    SensorData(ImuData& imuData, TofData (&tofData)[4]);
    ImuData& imuData;
    TofData tofData[4];
    long timestamp;
};

#endif
