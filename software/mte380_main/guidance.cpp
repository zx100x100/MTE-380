#include "guidance.h"

#define MAX_OUTPUT_POWER 100.0 // must be < 255

// for convenience
#define guidanceData gd

// constrain value to within + or - maximum
void constrain(float val, float maximum){
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

Segment::Segment(){

}


Line::Line(){
}

Path::Path(){
  segments = {
    Line()
  }
}

Guidance::Guidance(NavData& navData, CmdData& cmdData, Hms* hms):
  navData(navData),
  cmdData(cmdData),
  hms(hms)
{
  gd = GuidanceData_init_zero;
}

void Guidance::update(){
  path.updatePos();

  gd.segNum = path.segment();

  gd.vel = (navData.velX^2 + navData.velY^2+navData.velZ^2)^0.5;
  gd.velSetpoint = path.velSetpoint;
  float lastErrVel = gd.errVel;
  gd.errVel = gd.velSetpoint - gd.vel;
  gd.deltaT = micros() - lastTimestamp;
  gd.errVelD = (errVel - lastErrVel)/gd.deltaT;
  gd.errVelI += errVel * gd.deltaT;
  gd.velP = gd.errVel * gd.kP_vel;
  gd.velI = gd.errVelI * gd.kI_vel;
  gd.velD = gd.errVelD * gd.kD_vel;

  gd.leftOutputVel = gd.velP + gd.velI + gd.velD;
  gd.rightOutputVel = gd.leftOutputVel;

  gd.leftOutputVel = constrain(gd.leftOutputVel, MAX_OUTPUT_POWER)
  gd.rightOutputVel = constrain(gd.rightOutputVel, MAX_OUTPUT_POWER)


  float lastErrDrift = gd.errDrift;
  gd.errDrift = path.distFromPath();
  gd.errDriftD = (errDrift - lastErrDrift)/gd.deltaT;
  gd.errDriftI += errDrift * gd.deltaT;
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
  gd.leftTotalPID = constrain(gd.leftTotalPID, MAX_OUTPUT_POWER);
  gd.rightTotalPID = constrain(gd.rightTotalPID, MAX_OUTPUT_POWER);

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
  // Serial.print("gd.leftPower:"); Serial.println(gd.leftPower);
  // Serial.print("gd.rightPower:"); Serial.println(gd.rightPower);
}

GuidanceData& Guidance::getData(){
  return gd;
}
