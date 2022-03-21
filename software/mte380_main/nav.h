#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "hms.h"
#include "nav_data.pb.h"
#include "sensors.h"
#include "fusion.h"
#include "hms_and_cmd_data.pb.h"

struct TofPosition {
  float yaw;
  float x;
  float y;
};

class Nav{
  public:
    Nav(Sensors &sensors, CmdData* cmdData, Hms* hms);
    void update();
    void init();
    NavData& getData();
    NavData getPred(float delT);

  private:
    NavData navData;
    CmdData* cmdData;
    Fusion fusion;
    Sensors& sensors;
    Hms* hms;
    float gain[6] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2};
    int lastTofsCount[4] = {-1, -1, -1, -1};

    bool isValid(TofOrder tof);
    bool tofsUpdated();
    TofPosition calculateTof();
    NavData calculateImu();
    float deg2rad(float deg);
    float rad2deg(float deg);
    float cosd(float deg);
    float sind(float deg);
    float getTofFt(TofOrder tofNum);
    void updateEstimate(const NavData imuEstimate, const TofPosition tofEstimate, const NavData pred);
};

#endif
