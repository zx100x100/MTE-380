#include "Arduino.h"

#include "traj.h"
#include "math_utils.h"

#define CURVE_RADIUS 0.5 // tiles (AKA feet, they are litterally just feet)
#define CURVE_SPEED 0.12 // tiles/s
/* #define ACC 12 // tiles/s^2 (So this is just ft/s^2) */
/* #define VMAX 3 // tiles/s */
#define ACC 0.5 // tiles/s^2 (So this is just ft/s^2)
#define VMAX 0.3 // tiles/s
#define TRAP_SPEED 2 // tiles/s

// #define OFFSET_WALLS_FOR_SAFETY_TEMP 0 //0.3
#define OFFSET_WALLS_FOR_SAFETY_TEMP 0.3 //0.3

Subline::Subline(){}
Subline::Subline(float d1, float d4, float v1, float v4, Hms* hms):
  d1(d1),
  d4(d4),
  v1(v1),
  v4(v4),
  hms(hms)
{
  // this math is all on the whiteboard ----------------------------------------------
  if (d4 > d1){
    a = -ACC;
    vm = VMAX;
  }
  else{
    a = ACC;
    vm = -VMAX;
    v1 = -v1;
    v4 = -v4;
  }
  float aReq = fabs((pow(v4,2)-pow(v1,2))/(2*(d4-d1)))*(-sign(vm));
  if (fabs(aReq)>fabs(a)){
    a = aReq;
  }
  d2 = d1 + (pow(v1,2)-pow(VMAX,2))/(2*a);
  d3 = d4 + (pow(VMAX,2)-pow(v4,2))/(2*a);

  if (!(fabs(d1-d2)<fabs(d1-d3))){
    d3 = (pow(d1,2)-pow(d4,2))/(4*a)+(d1+d4)/2;
    d2 = d3;
  }
  // end of whiteboard math ----------------------------------------------------------
}

// get velocity based on trapezoidalAcceleration acceleration profile.
// d represents distance along the axis in which we are doing position based velocity
float Subline::trapezoidalAcceleration(float d){
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.println("trapezoidalAcceleration velocity"); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("d1: "); Serial.println(d1); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("d2: "); Serial.println(d2); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("d3: "); Serial.println(d3); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("d4: "); Serial.println(d4); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("v1: "); Serial.println(v1); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("v4: "); Serial.println(v4); } */
  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("d: "); Serial.println(d); } */

  float v; // were gonna return a velocity. whiteboard math-------------------------
  if(float_le(d*sign(vm), d1*sign(vm))){
    v = v1;
  }
  else if (float_le(d*sign(vm), d2*sign(vm))){
    v = pow(pow(v1,2)-2*a*(d-d1),0.5)*sign(vm);
  }
  else if (float_le(d*sign(vm), d3*sign(vm))){
    v = vm;
  }
  else if (float_le(d*sign(vm), d4*sign(vm))){
    v = pow(pow(v4,2)-2*a*(d4-d),0.5)*sign(vm);
  }
  else{
    v = v4;
  }

  /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("v: "); Serial.println(v); } */
  return v;
  // end of whiteboard math -------------------------------------------------------
  //
  //
  // If it doesnt make sense, look at the fucking whiteboard (not that it actually helps)
}

// Check if possition projected on to line is within segment
// use endcondition = -1 to look beyond the left bound, 1 to look beyond the right
bool Subline::isDOnLine(float d, int endCondition){
  if (endCondition == -1){
    return float_le(d,d4);
  }
  else if (endCondition == 1){
    return float_ge(d,d1);
  }
  return float_le(d,d4) && float_ge(d,d1);
}

// Get a velocity setpoint for the line given a position.
float Line::velSetpoint(float xp, float yp){
  float dp;
  if (horizontal){
    dp = xp;
  }
  else{
    dp = yp;
  }
  for (int i=0; i<nSublines; i++){
    int endCondition = 0;
    if (i == 0){
      if (orientation == 1){
        endCondition = -1;
      }
      else{
        endCondition = 1;
      }
    }
    else if (i == nSublines - 1){
      if (orientation == 1){
        endCondition = 1;
      }
      else{
        endCondition = -1;
      }
    }
    if (sublines[i].isDOnLine(dp, endCondition)){
      return sublines[i].trapezoidalAcceleration(dp);
    }
  }
  return 0;
}


// Create a line from a to b.
// either xa=xb or ya=yb, eg. line must be either horizontal or vertical
// xa may be larger or smaller than xb, and same in y axis. eg. may go backwards
Line::Line(float _xa, float _ya, float _xb, float _yb, float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS], Hms* hms):
  hms(hms)
{
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("Line::init"); }

  // horizontal line?
  horizontal = _ya == _yb;


  // orientation = -1 indicates a backwards line, ie. going from larger to smaller x or y
  if (horizontal){
    orientation = _xa > _xb ? -1 : 1;
  }
  else{
    orientation = _ya > _yb ? -1 : 1;
  }

  // if we want to turn on the spot, we need to do longer line segments. so we offset them by this variable
  // and multiply by orientation to offset in the correct direction.
  if (horizontal){
    xa = _xa+CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE*(orientation==-1?0.5:-0.5);
    xb = _xb-CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE*(orientation==-1?0.5:-0.5);
    ya = _ya;
    yb = _yb;
  }
  else{
    ya = _ya+CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE*(orientation==-1?0.5:-0.5);
    yb = _yb-CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE*(orientation==-1?0.5:-0.5);
    xa = _xa;
    xb = _xb;
  }
  
  for (int i=0; i<MAX_N_TRAPS; i++){
    if (isPointOnLine(trapX[i], trapY[i])){
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("Trap "); Serial.print(i); Serial.println(" found on line");}
      trapD[i] = horizontal?trapX[i]:trapY[i];
    }
    else{
      trapD[i] = -1;
    }
  }
  // distance of line endpoint a in the axis that the line is in. 
  // eg. for a horizontal line starting at (1,2), da = xa = 1
  float da;

  // see above
  float db;

  // dLow and dHigh are like da and db but based on numerical order whereas db may be less than da
  float dLow;
  float dHigh;
  if (horizontal){
    dLow = min(xa,xb);
    dHigh = max(xa,xb);
  }
  else{
    dLow = min(ya,yb);
    dHigh = max(ya,yb);
  }



  // VERY MAJOR TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
  // 
  // break the following code into functions because its pretty gross even though its arguably not bloated
  //
  //
  // !!!!!!!!!!!!!
  //

  // assign sublines. sublines are for trapezoidal velocity ramping between trapezoid corners.
  // the 4 trapezoid corners of the velocity are d1,d2,d3,d4 with velocities interpolated between.
  //
  // Save the d1 and d4 of the previous segment
  float last_d4 = dLow;
  float last_v4 = CURVE_SPEED;

  int lastSubline = -1; // keep track of how many sublines have been assigned
  while(true){
    float d1;
    float d4;
    float v1;
    float v4;

    d1 = last_d4;
    v1 = last_v4;

    // now, we could either be a trap subline, long trap subline, or regular subline.
    // also could be in either orientation.
    // loop through possible traps to find if any of them occur after d1 on the number line,
    // and if any are found, we need the nearest one.
    int nearestTrapInd = -1;
    for (int i=0; i<MAX_N_TRAPS; i++){
      float trap = trapD[i];
      if (trap == -1){continue;} // trap not on line
      if (trap > d1){
        if (nearestTrapInd == -1){
          nearestTrapInd = i;
        }
        else if (trap-d1 < trapD[nearestTrapInd]){
          nearestTrapInd = i;
        }
      }
    }
    /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("nearestTrapInd: "); Serial.println(nearestTrapInd); } */

    // if no trap was found, simply finish subline
    if (nearestTrapInd == -1){
      v4 = CURVE_SPEED;
      d4 = dHigh;
    }
    else{ // trap was found
      v4 = TRAP_SPEED;
      // could either be a trap in current subline, or after. determine which:
      if (float_eq(trapD[nearestTrapInd]-0.5,d1)){ // trap is the current subline
        d4 = d1+1; // will override this in for loop below if we find its a double trap

        // may want to determine if trap is actually the FIRST subline (for v1 or v4 or whatever)... but for now, eh
        // now, we need to check if this is a double length trap.
        for (int i=0; i<MAX_N_TRAPS; i++){
          float trap = trapD[i];
          if (trap == -1){continue;} // trap not on line
          if (float_eq(trap - trapD[nearestTrapInd], 1)){
            d4 = trap+0.5;
          }
        }
      }
      else{ // trap is after the current subline, so current subline needs to end with trapD
        d4 = trapD[nearestTrapInd]-0.5;
      }
    }

    // create a subline using the variables created in gross if statements above
    sublines[++lastSubline] = Subline(d1,d4,v1,v4,hms);

    if (d4 == dHigh){
      break;
    }

    last_d4 = d4;
    last_v4 = v4;
  }
  nSublines = lastSubline + 1;
}

SegmentType Line::getType(){
  return LINE;
}

bool Line::isPointOnLine(float xp, float yp){
  if (horizontal){
    if (yp == ya){
      return min(xa,xb) < xp && max(xa,xb) > xp;
    }
    return false;
  }
  if (xp == xa){
    return min(ya,yb) < yp && max(ya,yb) > xp;
  }
  return false;
}

// nearest distance to line
float Line::getDist(float xp, float yp){
  if (horizontal){
    if (orientation == -1){
      return yp - ya;
    }
    else{
      return ya - yp;
    }
  }
  else{
    if (orientation == -1){
      return xa - xp;
    }
    else{
      return xp - xa;
    }
  }
}

// check if we have ventured beyond the final bound of the line so we can move to next segment
bool Line::completed(float xp, float yp){
  if (horizontal){
    if (xb > xa){
      return xp > xb;
    }
    return xp < xb;
  }
  if (yb > ya){
    return yp > yb;
  }
  return yp < yb;
}

Curve::Curve(float xc, float yc, CornerType cornerType, Hms* hms):
  xc(xc), // center x
  yc(yc), // center y
  cornerType(cornerType),
  hms(hms)
{
}

SegmentType Curve::getType(){
  return CURVE;
}

// travel at constant speed on curves
float Curve::velSetpoint(float xp, float yp){
  return CURVE_SPEED;
}

// returns negative if inside circle, positive if outside circle
float Curve::getDist(float xp, float yp){
  return pow(pow(xp-xc,2)+pow(yp-yc,2),0.5) - CURVE_RADIUS;
}

// have we moved beyond the curve?
bool Curve::completed(float xp, float yp){
  if (cornerType == BL){
    return yp < yc;
  }
  else if (cornerType == BR_){
    return xp < xc;
  }
  else if (cornerType == TL){
    return xp > xc;
  }
  else{
    return yp > yc;
  }
}

Traj::Traj(){}
Traj::Traj(Hms* hms, GuidanceData* gd, CmdData* cmdData):
  hms(hms),
  gd(gd),
  cmdData(cmdData)
{
  nTraps = cmdData->nTraps;
  gd->segNum = 0;
}

void Traj::init(){
  /* Traj::init() */
  // THIS PART OF THE CODE HAS NEVER (NOT EVEN ONCE) HAD HEAP CORRUPTION AKA POISONING ISSUES!!!
  // BUT IF IT EVER DOES, MY FRIEND SAID THAT UNCOMMENTING THIS WOULD BE HELPFUL...

  /* heap_caps_check_integrity(MALLOC_CAP_8BIT, true); */

  segments[0] = new Line(4.5,5.5 -OFFSET_WALLS_FOR_SAFETY_TEMP,1+OFFSET_WALLS_FOR_SAFETY_TEMP, 5.5 -OFFSET_WALLS_FOR_SAFETY_TEMP,cmdData->trapX,cmdData->trapY,hms);
  segments[1] = new Curve(1,5,BL,hms); // BL = bottom left, etc.
  segments[2] = new Line(0.5+OFFSET_WALLS_FOR_SAFETY_TEMP,5- OFFSET_WALLS_FOR_SAFETY_TEMP,0.5+ OFFSET_WALLS_FOR_SAFETY_TEMP,1+OFFSET_WALLS_FOR_SAFETY_TEMP,cmdData->trapX,cmdData->trapY,hms);
  segments[3] = new Curve(1,1,TL,hms);
  segments[4] = new Line(1+ OFFSET_WALLS_FOR_SAFETY_TEMP,0.5+ OFFSET_WALLS_FOR_SAFETY_TEMP,5-OFFSET_WALLS_FOR_SAFETY_TEMP,0.5+OFFSET_WALLS_FOR_SAFETY_TEMP,cmdData->trapX,cmdData->trapY,hms);
  segments[5] = new Curve(5,1,TR,hms);
  segments[6] = new Line(5.5 - OFFSET_WALLS_FOR_SAFETY_TEMP/2,1+OFFSET_WALLS_FOR_SAFETY_TEMP/2,5.5-OFFSET_WALLS_FOR_SAFETY_TEMP/2,4-OFFSET_WALLS_FOR_SAFETY_TEMP/2,cmdData->trapX,cmdData->trapY,hms);
  segments[7] = new Curve(5,4,BR_,hms);
  segments[8] = new Line(5,4.5,2,4.5,cmdData->trapX,cmdData->trapY,hms);
  segments[9] = new Curve(2,4,BL,hms);
  segments[10] = new Line(1.5,4,1.5,2,cmdData->trapX,cmdData->trapY,hms);
  segments[11] = new Curve(2,2,TL,hms);
  segments[12] = new Line(2,1.5,4,1.5,cmdData->trapX,cmdData->trapY,hms);
  segments[13] = new Curve(4,2,TR,hms);
  segments[14] = new Line(4.5,2,4.5,3,cmdData->trapX,cmdData->trapY,hms);
  segments[15] = new Curve(4,3,BR_,hms);
  segments[16] = new Line(4,3.5,3,3.5,cmdData->trapX,cmdData->trapY,hms);
  segments[17] = new Curve(3,3,BL,hms);
  segments[18] = new Curve(3,3,TL,hms);
  segments[19] = new Line(3,2.5,3.5,2.5,cmdData->trapX,cmdData->trapY,hms);
}

// returns true if cmdData contains new traps or deleted existing ones
bool Traj::trapsChanged(){
  bool changed = cmdData->nTraps != nTraps;
  nTraps = cmdData->nTraps;
  return changed;
}

void Traj::updateTraps(){
  if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updateTraps()");

  // update traps by simply re-generating lines and passing in the trap arrays, then deleting the old lines
  for (int i=0; i < NUM_SEGMENTS; i++){
    if (segments[i]->getType() == LINE){
      Line* tempLine = static_cast<Line*>(segments[i]);
      Line* newLine = new Line(tempLine->xa, tempLine->ya, tempLine->xb, tempLine->yb, cmdData->trapX, cmdData->trapY, hms);
      delete segments[i];
      segments[i] = newLine;
    }
  }
}

// returns true if finished driving the track, false otherwise
bool Traj::updatePos(float xp, float yp){

  if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updatePos");

  bool alreadyAdvancedHowTheFuckIsThatPossible = false;

  while(segments[gd->segNum]->completed(xp, yp)){
    if (alreadyAdvancedHowTheFuckIsThatPossible){
      /* hms->logError(HmsData_Error_WTF_AHMAD, "skipped track segment!"); */
      if(hms->data.guidanceLogLevel >= 1){ Serial.println("wtf ahmad, skipped track segment???"); }
    }
    if (++gd->segNum >= NUM_SEGMENTS){
      Serial.println("Completed track!");
      gd->segNum = 0; // dont wanna go overflowing arrays now do we???? DO WE?????
      return true;
    }
    alreadyAdvancedHowTheFuckIsThatPossible = true;
    gd->lastCompletedSegmentTime = micros();
    /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("completion loop. segNum: "); Serial.println(gd->segNum); } */
  }
  return false;
}

// get distance from robot to trajectory (eg. drift)
float Traj::getDist(float xp, float yp){
  return segments[gd->segNum]->getDist(xp,yp);
}

// get velocity setpoint based on distance along trajectory
float Traj::getSetpointVel(float xp, float yp){
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("Traj::getSetpointVel"); }
  return segments[gd->segNum]->velSetpoint(xp, yp);
}
