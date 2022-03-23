#include "guidance.h"
#include "math_utils.h"

#define MAX_OUTPUT_POWER 70 // must be < 255
#define MAX_TURN_IN_PLACE_OUTPUT_POWER 70 // must be < 255

Guidance::Guidance(NavData& _navData, CmdData& _cmdData, Hms* _hms, Motors* motors, Nav* nav):
  navData(_navData),
  cmdData(_cmdData),
  hms(_hms),
  motors(motors),
  nav(nav)
{
  gd = GuidanceData_init_zero;
  prevRunState = cmdData.runState;
  traj = Traj(_hms, &gd, &_cmdData);
}

void Guidance::init(){
  traj.init();
}

void Guidance::update(){
  if (hms->data.guidanceLogLevel >= 2) Serial.println("Guidance::update()");


  // edge detection - switching into SIM mode, reset some variables
  if (cmdData.runState == CmdData_RunState_SIM && cmdData.runState != prevRunState){
    gd.segNum = 0; // reset segment number to 0, restart course
    prevRunState = cmdData.runState;
    gd.errVelI = 0;
    gd.errDriftI = 0;
    lastTimestamp = micros();
    return; // dont trust the position data on first tick of sim mode; just return
  }

  // edge detection - switching into AUTO mode, reset some variables
  if (cmdData.runState == CmdData_RunState_AUTO && cmdData.runState != prevRunState){
    gd.segNum = 0; // reset segment number to 0, restart course
    prevRunState = cmdData.runState;
    gd.errVelI = 0;
    gd.errDriftI = 0;
    lastTimestamp = micros();
    return; // dont trust the position data on first tick of auto mode either so just return
  }

  // reset previous runState for edge detection
  prevRunState = cmdData.runState;
  
  // if we are on a curve, and in teleop mode, we need to advance because its an undefined state for getting nav data
  // bc ahmad hasnt figured out how to do GPS during curves yet or whatever
  if (traj.segments[gd.segNum]->getType() == CURVE
      && CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE
      && cmdData.runState == CmdData_RunState_TELEOP){
    gd.segNum++;
  }


  // The current system for getting navigation updates relies on calling Nav from guidance, and informing
  // Nav what the current cardinal heading (UP/DOWN/LEFT/RIGHT) of the robot is so that it knows which
  // walls to look for.
  //
  // The system in Nav does not work for CURVE trajectory segments, so lock the Nav updates behind a check of
  // what the current segment's getType returns, to make sure its a LINE segment.
  //
  // In order to implement curve trajectories later, we will need a way of telling Nav what the fuck is happening.
  if (traj.segments[gd.segNum]->getType() == LINE){// && cmdData.runState == CmdData_RunState_AUTO){
    if(hms->data.guidanceLogLevel >= 2){ Serial.println("getting nav data from nav"); }
    Line* tempLine = static_cast<Line*>(traj.segments[gd.segNum]);
    heading_t enumShit = tempLine->horizontal?(tempLine->orientation==1?RIGHT:LEFT):(tempLine->orientation==1?DOWN:UP);
    nav->update(enumShit);
  }
  else if (cmdData.runState == CmdData_RunState_SIM){
    // This is pretty jank.
    // In simulator mode, need to set all the nav variables to whatever the simulator inputs were.
    // So, we give Nav a dummy update here and it will handle the rest by checking runState and handling SIM
    nav->update(UP);
  }

  if(hms->data.guidanceLogLevel >= 2){ Serial.print("Current segment number: "); Serial.println(gd.segNum); }
  gd.completedTrack = traj.updatePos(navData.posX, navData.posY);


  // A PID while(true) loop to turn in place. delete when curves are sexy
  if (CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE > 0 && cmdData.runState == CmdData_RunState_AUTO){
    if (traj.segments[gd.segNum]->getType() == CURVE){
      turnInPlace();
    }
  }

  // update trajectory if the trap layout has changed
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("checking traps"); }
  if (traj.trapsChanged()){
    if (hms->data.guidanceLogLevel >= 2) Serial.println("updating traps");
    traj.updateTraps();
  }
  
  // timestamp stuff for dt for PID calculations
  float curTimestamp = micros();
  gd.deltaT = curTimestamp - lastTimestamp;
  lastTimestamp = curTimestamp;

  // VELOCITY PID -----------------------------------------------------------------------
  gd.vel = pow(pow(navData.velX,2) + pow(navData.velY,2),0.5);
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("gd.vel: "); Serial.println(gd.vel); }

  // get setpoint velocity using trajectory
  gd.setpointVel = traj.getSetpointVel(navData.posX, navData.posY);

  float lastErrVel = gd.errVel;
  gd.errVel = gd.setpointVel - gd.vel;
  gd.errVelD = (gd.errVel - lastErrVel)*1000000/gd.deltaT;
  gd.errVelI = 0; // ADD INTEGRAL BACK IN LATER!!!! or like don't....
  gd.velP = gd.errVel * gd.kP_vel;
  gd.velI = gd.errVelI * gd.kI_vel;
  gd.velD = gd.errVelD * gd.kD_vel;

  gd.leftOutputVel = gd.velP + gd.velI + gd.velD;
  gd.rightOutputVel = gd.leftOutputVel;

  gd.leftOutputVel = constrainVal(gd.leftOutputVel, MAX_OUTPUT_POWER);
  gd.rightOutputVel = constrainVal(gd.rightOutputVel, MAX_OUTPUT_POWER);

  // DRIFT PID --------------------------------------------------------------------------
  float lastErrDrift = gd.errDrift;
  gd.errDrift = traj.getDist(navData.posX, navData.posY);
  gd.errDriftD = (gd.errDrift - lastErrDrift)*1000000/gd.deltaT;
  gd.errDriftI = 0; // ADD INTEGRAL BACK IN LATER!!!! or like don't....

  gd.driftP = gd.errDrift * gd.kP_drift;
  gd.driftI = gd.errDriftI * gd.kI_drift;
  gd.driftD = gd.errDriftD * gd.kD_drift;

  float driftOutput = gd.driftP + gd.driftI + gd.driftD;
  gd.rightOutputDrift = -driftOutput;
  gd.leftOutputDrift = driftOutput;


  if(hms->data.guidanceLogLevel >= 1){ Serial.print("gd.leftOutputDrift: "); Serial.println(gd.leftOutputDrift); }
  if(hms->data.guidanceLogLevel >= 1){ Serial.print("gd.rightOutputDrift: "); Serial.println(gd.rightOutputDrift); }


  // ADD THE PIDs TOGETHER AND DO SOME CONSTRAINING ------------------------------------------
  gd.leftTotalPID = gd.leftOutputVel + gd.leftOutputDrift;
  gd.rightTotalPID = gd.rightOutputVel + gd.rightOutputDrift;

  if(hms->data.guidanceLogLevel >= 1){ Serial.print("gd.leftTotalPID before constraining: "); Serial.println(gd.leftTotalPID); }
  if(hms->data.guidanceLogLevel >= 1){ Serial.print("gd.rightTotalPID before constraining: "); Serial.println(gd.rightTotalPID); }

  if (gd.rightTotalPID > MAX_OUTPUT_POWER){
    float spillover = gd.rightTotalPID/MAX_OUTPUT_POWER;
    gd.rightTotalPID /= spillover;
    gd.leftTotalPID /= spillover;
  }
  else if (gd.rightTotalPID < -MAX_OUTPUT_POWER){
    float spillover = -gd.rightTotalPID/MAX_OUTPUT_POWER;
    gd.rightTotalPID /= spillover;
    gd.leftTotalPID /= spillover;
  }
  else if (gd.leftTotalPID > MAX_OUTPUT_POWER){
    float spillover = gd.leftTotalPID/MAX_OUTPUT_POWER;
    gd.rightTotalPID /= spillover;
    gd.leftTotalPID /= spillover;
  }
  else if (gd.leftTotalPID < -MAX_OUTPUT_POWER){
    float spillover = -gd.leftTotalPID/MAX_OUTPUT_POWER;
    gd.rightTotalPID /= spillover;
    gd.leftTotalPID /= spillover;
  }
  gd.leftTotalPID = constrainVal(gd.leftTotalPID, MAX_OUTPUT_POWER);
  gd.rightTotalPID = constrainVal(gd.rightTotalPID, MAX_OUTPUT_POWER);

  if(hms->data.guidanceLogLevel >= 1){ Serial.print("gd.leftTotalPID after constraining: "); Serial.println(gd.leftTotalPID); }
  if(hms->data.guidanceLogLevel >= 1){ Serial.print("gd.rightTotalPID after constraining: "); Serial.println(gd.rightTotalPID); }

  // if in teleop mode, just send the motor values that the dashboard gives us
  if (cmdData.runState == CmdData_RunState_TELEOP){
    gd.leftPower = cmdData.leftPower;
    gd.rightPower = cmdData.rightPower;
  }
  // otherwise, use PID outputs
  else if (cmdData.runState == CmdData_RunState_AUTO){
    gd.leftPower = gd.leftTotalPID;
    gd.rightPower = gd.rightTotalPID;
  }
  else{
    gd.leftPower = 0;
    gd.rightPower = 0;
  }
}

  // A PID while(true) loop to turn in place. delete when curves are sexy
void Guidance::turnInPlace(){
  float startAngle = nav->getGyroAngle();
  float curAngle = startAngle;
  float threshhold = 5; // end loop when 5 degrees from donezo
  float angleDelta = 0;
  float error = 90;
  float lastError = error;
  float kp_turny = 1;
  float kd_turny = 0.2;
  float ki_turny = 0.0;
  float firstTimestamp = micros();
  float lastTimestamp = micros(); // zach I pinky promise that these two timestamps
  // will not be subtracted from each other and result in divide by zero errors.

  float curTimestamp;
  float deltaT;
  float errorD;
  float errorI;
  float P;
  float I;
  float D;
  float total;
  float newAngle;

  // theres a timeout dont worry
  while(abs(error)>threshhold){
    newAngle = nav->getGyroAngle();
    if (curAngle - newAngle > 300){ //300 since cur - new will loop over to 360 degrees, but not quite 360
      newAngle += 360;
    }
    curAngle = newAngle;
    angleDelta = curAngle - startAngle;
    error = 90 - angleDelta;
    curTimestamp = micros();

    // if it takes longer than 4 seconds to turn, you fucked up
    if (curTimestamp - firstTimestamp > 4000000){
      Serial.println("turn better next time please");
      break;
    }
    deltaT = curTimestamp - lastTimestamp;
    lastTimestamp = curTimestamp;
    errorD = (error - lastError)/deltaT;
    errorI += error * deltaT;
    P = error * kp_turny;
    I = errorI * ki_turny;
    D = errorD * kd_turny;

    total = P + I + D;
    total = constrainVal(P + I + D, MAX_TURN_IN_PLACE_OUTPUT_POWER);
    motors->setPower(total, -total);
  }
  gd.segNum++;
  return;
}

GuidanceData& Guidance::getData(){
  return gd;
}
