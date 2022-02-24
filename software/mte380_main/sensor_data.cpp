#include "sensor_data.h"

SensorData::SensorData(ImuData& imuData, TofData (&tofData)[4])
  :imuData(imuData)
   ,tofData(tofData)
   ,timestamp{0}
{}

