#ifndef GUIDANCE_H
#define GUIDANCE_H

#include "guidance_data.pb.h"
#include "nav_data.pb.h"
#include "cmd_data.pb.h"
#include "hms.h"

class Guidance{
  public:
    Guidance(NavData& navData, CmdData& cmdData, Hms* hms);
    void update();
    GuidanceData& getData();

  private:
    GuidanceData guidanceData;
    NavData& navData;
    CmdData& cmdData;
    Hms* hms;

    unsigned long lastTimestamp;
    unsigned long deltaT;
};

#endif
