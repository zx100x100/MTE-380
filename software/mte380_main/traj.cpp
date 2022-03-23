#include "Arduino.h"

#include "traj.h"
#include "math_utils.h"

#define CURVE_RADIUS 0.5 // tiles
#define CURVE_SPEED 0//1 // tiles/s
#define ACC 12 // tiles/s^2
#define VMAX 3 // tiles/s^2
#define TRAP_SPEED 2 // tiles/s

Subline::Subline(){}
Subline::Subline(float d1, float d4, float v1, float v4, Hms* hms):
  d1(d1),
  d4(d4),
  v1(v1),
  v4(v4),
  hms(hms)
{
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
  float aReq = abs((pow(v4,2)-pow(v1,2))/(2*(d4-d1)))*(-sign(vm));
  if (abs(aReq)>abs(a)){
    a = aReq;
  }
  d2 = d1 + (pow(v1,2)-pow(VMAX,2))/(2*a);
  d3 = d4 + (pow(VMAX,2)-pow(v4,2))/(2*a);

  if (!(abs(d1-d2)<abs(d1-d3))){
    d3 = (pow(d1,2)-pow(d4,2))/(4*a)+(d1+d4)/2;
    d2 = d3;
  }
}

float Subline::trapezoidal(float d){
  // Serial.println("trapezoidal");
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("trapezoidal"); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("d1: "); Serial.println(d1); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("d2: "); Serial.println(d2); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("d3: "); Serial.println(d3); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("d4: "); Serial.println(d4); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("v1: "); Serial.println(v1); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("v4: "); Serial.println(v4); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("d: "); Serial.println(d); }
  

  float v;
  if (float_le(d*sign(vm), d2*sign(vm))){
    v = pow(pow(v1,2)-2*a*(d-d1),0.5)*sign(vm);
  }
  else if (float_le(d*sign(vm), d3*sign(vm))){
    v = vm;
  }
  else{
    v = pow(pow(v4,2)-2*a*(d4-d),0.5)*sign(vm);
  }

  if(hms->data.guidanceLogLevel >= 2){ Serial.print("v: "); Serial.println(v); }
  return v;
}


bool Subline::isDOnLine(float d, int endCondition){
  if (endCondition == -1){
    return float_le(d,d4);
  }
  else if (endCondition == 1){
  }
    return float_ge(d,d1);
  }
  return float_le(d,d4) && float_ge(d,d1);
}

float Line::velSetpoint(float xp, float yp){
  float dp;
  if (horizontal){
    dp = xp;
  }
  else{
    dp = yp;
  }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.println("Line::velSetpoint"); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("nSublines: "); Serial.println(nSublines); }
  for (int i=0; i<nSublines; i++){
    // if(hms->data.guidanceLogLevel >= 2){ Serial.println("loop?"); }
    /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("dp: "); Serial.println(dp); } */
    /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[i].isDOnLine(dp): "); Serial.println(sublines[i].isDOnLine(dp)); } */
    if (sublines[i].isDOnLine(dp)){
      return sublines[i].trapezoidal(dp);
    }
  }
  return 0;
}

Line::Line(float _xa, float _ya, float _xb, float _yb, float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS], Hms* hms):
  hms(hms)
{
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("Line::init"); }
  horizontal = _ya == _yb;
  if (horizontal){
    orientation = _xa > _xb ? -1 : 1;
  }
  else{
    orientation = _ya > _yb ? -1 : 1;
  }
  if (horizontal){
    xa = _xa+BULLSHIT*(orientation==-1?0.5:-0.5);
    xb = _xb-BULLSHIT*(orientation==-1?0.5:-0.5);
    ya = _ya;
    yb = _yb;
  }
  else{
    ya = _ya+BULLSHIT*(orientation==-1?0.5:-0.5);
    yb = _yb-BULLSHIT*(orientation==-1?0.5:-0.5);
    xa = _xa;
    xb = _xb;
  }
  
  float included[MAX_N_TRAPS];

  for (int i=0; i<MAX_N_TRAPS; i++){
    if (isPointOnLine(trapX[i], trapY[i])){
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("Trap "); Serial.print(i); Serial.println(" found on line");}
      trapD[i] = horizontal?trapX[i]:trapY[i];
    }
    else{
      trapD[i] = -1;
    }
  }
  float da;
  float db;
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
  float last_d1;
  float last_d4 = dLow;
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("dLow: "); Serial.println(dLow); }
  float last_v1;
  float last_v4 = CURVE_SPEED;
  int lastSubline = -1;
  while(true){
    // assign start
    float d1;
    float d4;
    float v1;
    float v4;

    // always
    d1 = last_d4;
    if(hms->data.guidanceLogLevel >= 2){ Serial.print("d1: "); Serial.println(d1); }
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
    if(hms->data.guidanceLogLevel >= 2){ Serial.print("nearestTrapInd: "); Serial.println(nearestTrapInd); }

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
    sublines[++lastSubline] = Subline(d1,d4,v1,v4,hms);

    if (d4 == dHigh){
      break;
    }

    last_d1 = d1;
    last_d4 = d4;
    last_v1 = v1;
    last_v4 = v4;
  }
  nSublines = lastSubline + 1;

  if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].d1;: "); Serial.println(sublines[0].d1); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].d4;: "); Serial.println(sublines[0].d4); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].isDOnLine(4.5): "); Serial.println(sublines[0].isDOnLine(4)); }
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

float Curve::velSetpoint(float xp, float yp){
  return CURVE_SPEED;
}

// returns negative if inside circle, positive if outside circle
float Curve::getDist(float xp, float yp){
  return pow(pow(xp-xc,2)+pow(yp-yc,2),0.5) - CURVE_RADIUS;
}

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

  segments[0] = new Line(4.5,5.5,1,5.5,cmdData->trapX,cmdData->trapY,hms);
  /* segments[0] = new Line(4.5,5,1,5,cmdData->trapX,cmdData->trapY,hms); */
  segments[1] = new Curve(1,5,BL,hms);
  segments[2] = new Line(0.5,5,0.5,1,cmdData->trapX,cmdData->trapY,hms);
  segments[3] = new Curve(1,1,TL,hms);
  segments[4] = new Line(1,0.5,5,0.5,cmdData->trapX,cmdData->trapY,hms);
  segments[5] = new Curve(5,1,TR,hms);
  segments[6] = new Line(5.5,1,5.5,4,cmdData->trapX,cmdData->trapY,hms);
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

// returns true if cmdData contanis new traps
bool Traj::trapsChanged(){
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("cmdData-nTraps: "); Serial.println(cmdData->nTraps); }
  if(hms->data.guidanceLogLevel >= 2){ Serial.print("nTraps: "); Serial.println(nTraps); }
  bool changed = cmdData->nTraps != nTraps;
  nTraps = cmdData->nTraps;
  return changed;
}

void Traj::updateTraps(){
  if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updateTraps()");
  for (int i=0; i < NUM_SEGMENTS; i++){
    if (segments[i]->getType() == LINE){
      /* *segments[i] = copyAndRecalculateTraps(static_cast<Line*>(segments[i]), cmdData->trapX, cmdData->trapY, hms); */
  /* return Line(line->xa, line->ya, line->xb, line->yb, trapX, trapY, line->hms); */
      Line* tempLine = static_cast<Line*>(segments[i]);
      Line* newLine = new Line(tempLine->xa, tempLine->ya, tempLine->xb, tempLine->yb, cmdData->trapX, cmdData->trapY, hms);
      delete segments[i];
      segments[i] = newLine;

      // Line* tempLine = static_cast<Line*>(segments[i]);
      // tempLine -> updateTraps(cmdData->trapX, cmdData->trapY);
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->nSublines: "); Serial.println(newLine->nSublines); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[0].d1: "); Serial.println(newLine->sublines[0].d1); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[0].d2: "); Serial.println(newLine->sublines[0].d2); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[0].d3: "); Serial.println(newLine->sublines[0].d3); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[0].d4: "); Serial.println(newLine->sublines[0].d4); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[0].v1: "); Serial.println(newLine->sublines[0].v1); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[0].v4: "); Serial.println(newLine->sublines[0].v4); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[1].d1: "); Serial.println(newLine->sublines[1].d1); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[1].d2: "); Serial.println(newLine->sublines[1].d2); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[1].d3: "); Serial.println(newLine->sublines[1].d3); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[1].d4: "); Serial.println(newLine->sublines[1].d4); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[1].v1: "); Serial.println(newLine->sublines[1].v1); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[1].v4: "); Serial.println(newLine->sublines[1].v4); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[2].d1: "); Serial.println(newLine->sublines[2].d1); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[2].d2: "); Serial.println(newLine->sublines[2].d2); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[2].d3: "); Serial.println(newLine->sublines[2].d3); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[2].d4: "); Serial.println(newLine->sublines[2].d4); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[2].v1: "); Serial.println(newLine->sublines[2].v1); }
      if(hms->data.guidanceLogLevel >= 2){ Serial.print("newLine->sublines[2].v4: "); Serial.println(newLine->sublines[2].v4); }
      // while(true){}
    }
  }
}

// returns true if finished driving the track, false otherwise
bool Traj::updatePos(float xp, float yp){

  if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updatePos");
  bool advanced = false;

  while(segments[gd->segNum]->completed(xp, yp)){
    /* if(hms->data.guidanceLogLevel >= 2){ Serial.print("gd->segNum: "); Serial.println(gd->segNum); } */
    /* if (hms->data.guidanceLogLevel >= 2) Serial.println("blah1"); */
    if (advanced){
      /* if (hms->data.guidanceLogLevel >= 2) Serial.println("blah2"); */
      /* hms->logError(HmsData_Error_WTF_AHMAD, "skipped track segment!"); */
    }
    /* if (hms->data.guidanceLogLevel >= 2) Serial.println("blah3"); */
    if (gd->segNum++ >= NUM_SEGMENTS){
      Serial.println("Completed track!");
      gd->segNum--;
      return true;
    }
    advanced = true;
    if(hms->data.guidanceLogLevel >= 2){ Serial.print("completion loop. segNum: "); Serial.println(gd->segNum); }
  }
  /* if (hms->data.guidanceLogLevel >= 2) Serial.println("done updatePos"); */
  return false;
}

float Traj::getDist(float xp, float yp){
  return segments[gd->segNum]->getDist(xp,yp);
}

float Traj::getSetpointVel(float xp, float yp){
  if(hms->data.guidanceLogLevel >= 2){ Serial.println("Traj::getSetpointVel"); }
  return segments[gd->segNum]->velSetpoint(xp, yp);
}
