#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "nav_data.pb.h"
#include "sensors.h"
#include "fusion.h"

class Nav{
  public:
    Nav(Sensors &sensors);
    void update();
    void updateImu();
    void init();
    NavData& getData();

  private:
    NavData navData;
    Sensors& sensors;
    Fusion fusion;
};

#endif
