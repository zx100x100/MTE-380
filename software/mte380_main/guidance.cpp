#include "guidance.h"

Guidance::Guidance(NavData& navData, CmdData& cmdData, Hms* hms):
  navData(navData),
  cmdData(cmdData),
  hms(hms)
{
  guidanceData = GuidanceData_init_zero;
}

void Guidance::update(){
  // calculate everything, then
  float outputLeft = 0;
  float outputRight = 0; // TODO replace w/ real vals

  if (cmdData.runState == CmdData_RunState_TELEOP){
    guidanceData.leftPower = cmdData.leftPower;
    guidanceData.rightPower = cmdData.rightPower;
    guidanceData.propPower = cmdData.propPower;
  }
  else if (cmdData.runState == CmdData_RunState_AUTO){
    guidanceData.leftPower = outputLeft;
    guidanceData.rightPower = outputRight;
    guidanceData.propPower = 0; // temp
  }
  else{
    guidanceData.leftPower = 0;
    guidanceData.rightPower = 0;
    guidanceData.propPower = 0;
  }
  // Serial.print("guidanceData.leftPower:"); Serial.println(guidanceData.leftPower);
  // Serial.print("guidanceData.rightPower:"); Serial.println(guidanceData.rightPower);
}

GuidanceData& Guidance::getData(){
  return guidanceData;
}
