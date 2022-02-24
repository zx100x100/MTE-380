#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "nav_data.pb.h"
#include "sensor_data.h"
#include "fusion.h"

class Nav{
  public:
    Nav(SensorData &sensorData);
    void update();
    void updateImu();
    void init();
    NavData& getData();

  private:
    NavData navData;
    SensorData& sensorData;
    Fusion fusion;
};

#endif
