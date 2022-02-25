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
  hms->hmsData.batteryVoltage = 1.0;

  if (cmdData.runState == CmdData_RunState_TELEOP){
    guidanceData.leftPower = cmdData.teleop.leftPower;
    guidanceData.rightPower = cmdData.teleop.rightPower;
  }
  else if (cmdData.runState == CmdData_RunState_AUTO){
    guidanceData.leftPower = outputLeft;
    guidanceData.rightPower = outputRight;
  }
  Serial.print("guidanceData.leftPower:"); Serial.println(guidanceData.leftPower);
  Serial.print("guidanceData.rightPower:"); Serial.println(guidanceData.rightPower);

  // FAKE DATA
  guidanceData.errVel = 1;
  guidanceData.errDrift = 2;
  guidanceData.kP_vel = 3; // PID constants for velocity PID
  guidanceData.kI_vel = 4;
  guidanceData.kD_vel = 5;
  guidanceData.kP_drift = 6; // PID constants for drift PID
  guidanceData.kI_drift = 7;
  guidanceData.kD_drift = 8;
  guidanceData.leftOutputVel = 9; // left motor output from velocity PID
  guidanceData.rightOutputVel = 10; // right motor output from velocity PID
  guidanceData.leftOutputDrift = 11; // left motor output from drift PID
  guidanceData.rightOutputDrift = 12; // right motor output from drift PID
  guidanceData.leftPower = 13; // combined left motor output from vel and drift PIDs
  guidanceData.rightPower = 14; // combined right motor output from vel and drift PIDs
  guidanceData.segNum = 15; // current arena segment number
}

GuidanceData& Guidance::getData(){
  return guidanceData;
}
