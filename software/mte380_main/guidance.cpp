#include "guidance.h"

Guidance::Guidance(NavData& navData, CmdData& cmdData)
: navData(navData)
, cmdData(cmdData){
  guidanceData = GuidanceData_init_zero;
}

void Guidance::update(){
  // calculate everything, then
  float outputLeft = 0;
  float outputRight = 0; // TODO replace w/ real vals

  if (cmdData.runState == RunState::RunState_TELEOP){
    guidanceData.leftPower = cmdData.teleop.leftPower;
    guidanceData.rightPower = cmdData.teleop.rightPower;
  }
  else if (cmdData.runState == RunState::RunState_AUTO){
    guidanceData.leftPower = outputLeft;
    guidanceData.rightPower = outputLeft;
  }
}

GuidanceData& Guidance::getData(){
  return guidanceData;
}
