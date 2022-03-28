#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "hms.h"
#include "nav_data.pb.h"
#include "sensors.h"
#include "hms_and_cmd_data.pb.h"
#include "guidance_data.pb.h"
#include <imuFilter.h>

struct TofPos {
  float yaw; bool yawValid;
  float x; bool yValid;
  float y; bool xValid;
};

class Nav{
  public:
    Nav(Sensors &sensors, CmdData* cmdData, Hms* hms);
    void update(GuidanceData_Heading heading);
    void init();
    NavData& getData();
    void updatePred();
    float getGyroAngle();

  private:
    NavData navData;
    NavData predNavData;  // This is a prediction based on previous navData and delta time
    CmdData* cmdData;
    Sensors& sensors;
    Hms* hms;
    TofPos tofPos;
    float gain[6] = {0.2, 0.2, 1, 1, 0, 0};
    int lastTofsCount[4] = {-1, -1, -1, -1};

    bool isValid(TofOrder tof);
    bool tofsUpdated();
    void updateTof(GuidanceData_Heading heading);
    NavData calculateImu();
    //mTODO move to math utils
    float getTofFt(TofOrder tofNum);
    void updateEstimate(const NavData pred);
    float predToFront(GuidanceData_Heading heading);

    // IMU Filter Gain
    constexpr static float imuGain = 0.1;
    imuFilter<&imuGain> fusion;

    float lastBackSensorVal;
};

#endif
