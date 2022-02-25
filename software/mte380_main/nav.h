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
    Hms* hms; // todo move back to private

  private:
    NavData navData;
    Fusion fusion;
    Sensors& sensors;
};

#endif
