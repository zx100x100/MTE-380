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
    Serial.println("tele");
    guidanceData.leftPower = cmdData.teleop.leftPower;
    guidanceData.rightPower = cmdData.teleop.rightPower;
  }
  else if (cmdData.runState == CmdData_RunState_AUTO){
    Serial.println("auto");
    guidanceData.leftPower = outputLeft;
    guidanceData.rightPower = outputRight;
  }
  else{
    guidanceData.leftPower = 0;
    guidanceData.rightPower = 0;
  }
  // Serial.print("guidanceData.leftPower:"); Serial.println(guidanceData.leftPower);
  // Serial.print("guidanceData.rightPower:"); Serial.println(guidanceData.rightPower);
}

GuidanceData& Guidance::getData(){
  return guidanceData;
}
