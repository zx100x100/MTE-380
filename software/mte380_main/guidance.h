#ifndef GUIDANCE_H
#define GUIDANCE_H

#include "guidance_data.pb.h"
#include "nav_data.pb.h"
#include "hms_and_cmd_data.pb.h"
#include "hms.h"
#include "traj.h"

class Guidance{
  public:
    Guidance(NavData& navData, CmdData& cmdData, Hms* hms);
    void init();
    void update();
    GuidanceData& getData();

  private:
    GuidanceData gd;
    NavData& navData;
    CmdData& cmdData;
    Hms* hms;

    unsigned long lastTimestamp;
    unsigned long deltaT;
    /* float lastTimestamp; */
    /* float deltaT; */

    CmdData_RunState prevRunState;

    Traj traj;
};

#endif
