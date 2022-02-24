#ifndef GUIDANCE_H
#define GUIDANCE_H

#include "guidance_data.pb.h"
#include "nav_data.pb.h"
#include "cmd_data.pb.h"

class Guidance{
  public:
    Guidance();
    Guidance(NavData& navData, CmdData& cmdData);
    void update();
    GuidanceData& getData();

  private:
    GuidanceData guidanceData;
    NavData& navData;
    CmdData& cmdData;
};

#endif
