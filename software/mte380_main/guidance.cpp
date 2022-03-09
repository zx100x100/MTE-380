#include "guidance.h"

#define MAX_OUTPUT_POWER 100.0 // must be < 255

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
  traj = Traj(_hms, &gd, &_cmdData);
}

void Guidance::init(){
}

void Guidance::update(){
  if (hms->data.guidanceLogLevel >= 2) Serial.println("Guidance::update()");
  gd.completedTrack = traj.updatePos(navData.posX, navData.posY);
  gd.vel = pow(pow(navData.velX,2) + pow(navData.velY,2)+pow(navData.velZ,2),0.5);

  gd.leftPower = 0;
  gd.rightPower = 0;
  gd.propPower = 0;
  return;

  if (traj.trapsChanged()){
    traj.updateTraps();
  }

  gd.setpointVel = traj.getSetpointVel(navData.posX, navData.posY);
  float lastErrVel = gd.errVel;
  gd.errVel = gd.setpointVel - gd.vel;
  gd.deltaT = micros() - lastTimestamp;
  gd.errVelD = (gd.errVel - lastErrVel)/gd.deltaT;
  gd.errVelI += gd.errVel * gd.deltaT;
  gd.velP = gd.errVel * gd.kP_vel;
  gd.velI = gd.errVelI * gd.kI_vel;
  gd.velD = gd.errVelD * gd.kD_vel;

  gd.leftOutputVel = gd.velP + gd.velI + gd.velD;
  gd.rightOutputVel = gd.leftOutputVel;

  gd.leftOutputVel = constrainVal(gd.leftOutputVel, MAX_OUTPUT_POWER);
  gd.rightOutputVel = constrainVal(gd.rightOutputVel, MAX_OUTPUT_POWER);


  float lastErrDrift = gd.errDrift;
  gd.errDrift = traj.getDist(navData.posX, navData.posY);
  gd.errDriftD = (gd.errDrift - lastErrDrift)/gd.deltaT;
  gd.errDriftI += gd.errDrift * gd.deltaT;
  gd.driftP = gd.errDrift * gd.kP_drift;
  gd.driftI = gd.errDriftI * gd.kI_drift;
  gd.driftD = gd.errDriftD * gd.kD_drift;

  float driftOutput = gd.driftP + gd.driftI + gd.driftD;
  gd.rightOutputDrift = driftOutput;
  gd.leftOutputDrift = -driftOutput;

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

  /* if (cmdData.runState == CmdData_RunState_TELEOP){ */
    /* gd.leftPower = cmdData.leftPower; */
    /* gd.rightPower = cmdData.rightPower; */
    /* gd.propPower = cmdData.propPower; */
  /* } */
  /* else if (cmdData.runState == CmdData_RunState_AUTO){ */
    /* gd.leftPower = outputLeft; */
    /* gd.rightPower = outputRight; */
    /* gd.propPower = 0; // temp */
  /* } */
  /* else{ */
    /* gd.leftPower = 0; */
    /* gd.rightPower = 0; */
    /* gd.propPower = 0; */
    /* if (cmdData.runState == CmData_RunState_SIM){ */
      /* float FAKE_VELOCITY_SCALE_FACTOR = 0.01; */
      /* float speed = FAKE_VELOCITY_SCALE_FACTOR * (cmData.leftPower+cmData.rightPower)/2; */
      /* navData.posX =  */
      /* if cmData.leftPower */
    /* } */
  /* } */

  // Serial.print("gd.leftPower:"); Serial.println(gd.leftPower);
  // Serial.print("gd.rightPower:"); Serial.println(gd.rightPower);
  // calculate everything, then
  // float outputLeft = 0;
  // float outputRight = 0; // TODO replace w/ real vals

  // if (cmdData.runState == CmdData_RunState_TELEOP){
    // if (hms->data.guidanceLogLevel >= 2){
      // Serial.println("teleop");
    // }
    // guidanceData.leftPower = cmdData.leftPower;
    // guidanceData.rightPower = cmdData.rightPower;
    // guidanceData.propPower = cmdData.propPower;
  // }
  // else if (cmdData.runState == CmdData_RunState_AUTO){
    // guidanceData.leftPower = outputLeft;
    // guidanceData.rightPower = outputRight;
    // guidanceData.propPower = 0; // temp
  // }
  // else{
    // if (hms->data.guidanceLogLevel >= 2){
      // Serial.println("stopped");
    // }
    // guidanceData.leftPower = 0;
    // guidanceData.rightPower = 0;
    // guidanceData.propPower = 0;
  // }
  // if (hms->data.guidanceLogLevel >= 2){
    // Serial.print("guidanceData.leftPower:"); Serial.println(guidanceData.leftPower);
    // Serial.print("guidanceData.rightPower:"); Serial.println(guidanceData.rightPower);
  // }
}

GuidanceData& Guidance::getData(){
  return gd;
}
