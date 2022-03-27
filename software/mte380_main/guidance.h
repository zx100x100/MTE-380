#ifndef GUIDANCE_H
#define GUIDANCE_H

#include "guidance_data.pb.h"
#include "nav_data.pb.h"
#include "hms_and_cmd_data.pb.h"
#include "hms.h"
#include "traj.h"
#include "motors.h"
#include "nav.h"
#include "sensors.h"

class Guidance{
  public:
    Guidance(NavData& navData, CmdData& cmdData, Hms* hms, Motors* motors, Nav* nav, Sensors* sensors);
    void init();
    void update();
    GuidanceData& getData();
    void turnInPlace();
    Motors* motors;
    GuidanceData gd;

  private:
    NavData& navData;
    CmdData& cmdData;
    Hms* hms;
    Nav* nav;
    Sensors* sensors;

    unsigned long lastTimestamp;
    unsigned long firstTimestamp;

    CmdData_RunState prevRunState;

    Traj traj;

    float lastPosX;
    float lastPosY;
    float lastVelX;
    float lastVelY;
    float velX;
    float velY;

    int consecutiveBadNavDataTicks;
};

#endif
