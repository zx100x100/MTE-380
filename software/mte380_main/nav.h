#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "hms.h"
#include "nav_data.pb.h"
#include "sensors.h"
#include "fusion.h"

class Nav{
  public:
    Nav(Sensors &sensors, Hms* hms);
    void update();
    void updateImu();
    void init();
    NavData& getData();

  private:
    NavData navData;
    Fusion fusion;
    Sensors& sensors;
    Hms* hms;
};

#endif
