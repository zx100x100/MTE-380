#ifndef GUIDANCE_H
#define GUIDANCE_H

#include "guidance_data.pb.h"
#include "nav_data.pb.h"
#include "hms_and_cmd_data.pb.h"
#include "hms.h"
#include "traj.h"
#include "motors.h"
#include "nav.h"

class Guidance{
  public:
    Guidance(NavData& navData, CmdData& cmdData, Hms* hms, Motors* motors, Nav* nav);
    void init();
    void update();
    GuidanceData& getData();
    void turnInPlace();
    Motors* motors;

  private:
    GuidanceData gd;
    NavData& navData;
    CmdData& cmdData;
    Hms* hms;
    Nav* nav;

    unsigned long lastTimestamp;

    CmdData_RunState prevRunState;

    Traj traj;
};

#endif
