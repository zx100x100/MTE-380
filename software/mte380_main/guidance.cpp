#include "guidance.h"

#define MAX_OUTPUT_POWER 255.0 // must be < 255

// constrainVal value to within + or - maximum
float constrainVal(float val, float maximum){
  if (val > 0){
    if (val > maximum){
      return maximum;
    }
    return val;
  }
  if (val < -maximum){
    return -maximum;
  }
  return val;
}

Guidance::Guidance(NavData& _navData, CmdData& _cmdData, Hms* _hms):
  navData(_navData),
  cmdData(_cmdData),
  hms(_hms)
{
  gd = GuidanceData_init_zero;
  prevRunState = cmdData.runState;
  traj = Traj(_hms, &gd, &_cmdData);
}

void Guidance::init(){
  traj.init();
  /* gd.kP_vel = 5.0; */
  /* gd.kP_drift = 150.0; */
}

void Guidance::update(){
  if (hms->data.guidanceLogLevel >= 2) Serial.println("Guidance::update()");
  if (cmdData.runState == CmdData_RunState_SIM && cmdData.runState != prevRunState){
    // edge detection - switching into SIM mode
    gd.segNum = 0;
    prevRunState = cmdData.runState;
    gd.errVelI = 0;
    gd.errDriftI = 0;
    lastTimestamp = micros();
    return; // dont trust the position data on first tick of sim mode
  }
  prevRunState = cmdData.runState;

  // if (cmdData.runState != CmdData_RunState_SIM){
    // if (hms->data.guidanceLogLevel >= 2) Serial.println("returning");
    // return;
  // }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("navData.posX: "); Serial.println(navData.posX); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("navData.posY: "); Serial.println(navData.posY); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("navData.velX: "); Serial.println(navData.velX); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("navData.velY: "); Serial.println(navData.velY); }

  if(hms->data.guidanceLogLevel >= 2){ Serial.print("gd.segNum: "); Serial.println(gd.segNum); }


  gd.completedTrack = traj.updatePos(navData.posX, navData.posY);
  gd.vel = pow(pow(navData.velX,2) + pow(navData.velY,2)+pow(navData.velZ,2),0.5);
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("gd.vel: "); Serial.println(gd.vel); }

  gd.leftPower = 0;
  gd.rightPower = 0;
  gd.propPower = 0;

  if(hms->data.guidanceLogLevel >= 2){ Serial.println("checking traps"); }
  if (traj.trapsChanged()){
    if (hms->data.guidanceLogLevel >= 2) Serial.println("updating traps");
    traj.updateTraps();
  }
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("returning"); }

  gd.setpointVel = traj.getSetpointVel(navData.posX, navData.posY);
  float lastErrVel = gd.errVel;
  gd.errVel = gd.setpointVel - gd.vel;
  // float curTimestamp = micros()*1.0///1000000.0;
  float curTimestamp = micros();
  gd.deltaT = curTimestamp - lastTimestamp;
  lastTimestamp = curTimestamp;
  gd.errVelD = (gd.errVel - lastErrVel)/gd.deltaT;
  // gd.errVelI += gd.errVel * gd.deltaT;
  gd.errVelI = 0; // ADD INTEGRAL BACK IN LATER!!!!
  gd.velP = gd.errVel * gd.kP_vel;
  gd.velI = gd.errVelI * gd.kI_vel;
  gd.velD = gd.errVelD * gd.kD_vel;

  gd.leftOutputVel = gd.velP + gd.velI + gd.velD;
  gd.rightOutputVel = gd.leftOutputVel;

  gd.leftOutputVel = constrainVal(gd.leftOutputVel, MAX_OUTPUT_POWER);
  gd.rightOutputVel = constrainVal(gd.rightOutputVel, MAX_OUTPUT_POWER);


  float lastErrDrift = gd.errDrift;
  gd.errDrift = traj.getDist(navData.posX, navData.posY);
  gd.errDriftD = (gd.errDrift - lastErrDrift)*1000*1000/gd.deltaT;
  // gd.errDriftI += gd.errDrift * gd.deltaT;
  gd.errDriftI = 0; // ADD INTEGRAL BACK IN LATER!!!!
  gd.driftP = gd.errDrift * gd.kP_drift;
  gd.driftI = gd.errDriftI * gd.kI_drift;
  gd.driftD = gd.errDriftD * gd.kD_drift;

  float driftOutput = gd.driftP + gd.driftI + gd.driftD;
  gd.rightOutputDrift = -driftOutput;
  gd.leftOutputDrift = driftOutput;

  gd.leftTotalPID = gd.leftOutputVel + gd.leftOutputDrift;
  gd.rightTotalPID = gd.rightOutputVel + gd.rightOutputDrift;

  if (gd.rightTotalPID > MAX_OUTPUT_POWER){
    float spillover = gd.rightTotalPID-MAX_OUTPUT_POWER;
    gd.rightTotalPID -= spillover;
    gd.leftTotalPID += spillover;
  }
  else if (gd.rightTotalPID < -MAX_OUTPUT_POWER){
    float spillover = -gd.rightTotalPID-MAX_OUTPUT_POWER;
    gd.rightTotalPID += spillover;
    gd.leftTotalPID -= spillover;
  }
  else if (gd.leftTotalPID > MAX_OUTPUT_POWER){
    float spillover = gd.leftTotalPID-MAX_OUTPUT_POWER;
    gd.rightTotalPID += spillover;
    gd.leftTotalPID -= spillover;
  }
  else if (gd.leftTotalPID < -MAX_OUTPUT_POWER){
    float spillover = -gd.leftTotalPID-MAX_OUTPUT_POWER;
    gd.rightTotalPID -= spillover;
    gd.leftTotalPID += spillover;
  }
  gd.leftTotalPID = constrainVal(gd.leftTotalPID, MAX_OUTPUT_POWER);
  gd.rightTotalPID = constrainVal(gd.rightTotalPID, MAX_OUTPUT_POWER);
  if (hms->data.guidanceLogLevel >= 2) Serial.println("done guidance::update");

  if (cmdData.runState == CmdData_RunState_TELEOP){
    gd.leftPower = cmdData.leftPower;
    gd.rightPower = cmdData.rightPower;
  }
  else if (cmdData.runState == CmdData_RunState_AUTO){
    gd.leftPower = gd.leftTotalPID;
    gd.rightPower = gd.rightTotalPID;
  }
  else{
    gd.leftPower = 0;
    gd.rightPower = 0;
  }
}

GuidanceData& Guidance::getData(){
  return gd;
}
