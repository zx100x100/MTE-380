#include "traj.h"
#include "Arduino.h"

#define CURVE_RADIUS 0.5 // tiles
#define CURVE_SPEED 5 // tiles/s
#define ACC 5 // tiles/s^2
#define VMAX 6 // tiles/s^2
#define TRAP_SPEED 4 // tiles/s
#define EPSILON 0.000001 // for float equality

Line copyAndRecalculateTraps(Line* line, float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS]){
  return Line(line->xa, line->ya, line->xb, line->yb, trapX, trapY);
}


Subline::Subline(){}
Subline::Subline(float d1, float d4, float v1, float v4):
  d1(d1),
  d4(d4),
  v1(v1),
  v4(v4)
{
  d2 = (pow(VMAX,2) - pow(v1,2))/(2*ACC) - d1;
  d3 = d4 - (pow(VMAX,2)-pow(v4,2))/(2*ACC);
  dt = 0.5*((pow(v4,2)-pow(v1,2))/(2*ACC) + d1 + d4);
}

float Subline::trapezoidal(float d){
  if (d3 < d2){
    if (d < dt){
      return pow(2*ACC*(d-d1)+pow(v1,2),0.5);
    }
    return pow(pow(v4,2)-2*ACC*(d4-d),0.5);
  }
  if (d<d2){
      return pow(2*ACC*(d-d1)+pow(v1,2),0.5);
  }
  if (d<d3){
    return VMAX;
  }
  return pow(pow(v4,2)-2*ACC*(d4-d),0.5);
}


bool Subline::isDOnLine(float d){
  return d < d4 && d >= d1;
}

float Line::velSetpoint(float xp, float yp){
  float dp;
  if (horizontal){
    dp = xp;
  }
  else{
    dp = yp;
  }
  for (int i=0; i<nSublines; i++){
    if (sublines[i].isDOnLine(dp)){
      if (orientation == -1){
        // if the line segment goes bottom to top or right to left, that means we reversed our d1 and d4
        // which means that we need to mirror our d across the center of the line segment.
        float fromStart = dp - sublines[i].d1; 
        /* now make d be the same distance from d4 as it was from d1: */
        dp = sublines[i].d4 - fromStart;
      }
      return sublines[i].trapezoidal(dp);
    }
  }
  // raise HMS error????????? TODO
  return 0;
}

Line::Line(float xa, float ya, float xb, float yb, float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS]):
  xa(xa),
  ya(ya),
  xb(xb),
  yb(yb)
{
  horizontal = ya == yb;
  if (horizontal){
    orientation = xa > xb ? -1 : 1;
  }
  else{
    orientation = ya > yb ? -1 : 1;
  }
  
  float included[MAX_N_TRAPS] = {-1};
  nTraps = 0;

  for (int i=0; i<MAX_N_TRAPS; i++){
    if (isPointOnLine(trapX[i], trapY[i])){
      included[i] = 0;
      nTraps++;
    }
  }
  int nIncluded = 0;
  while(nIncluded < nTraps){
    for (int i=0; i<MAX_N_TRAPS; i++){
      // candidate i.
      // must be: not already in arrya (order 1)
      //          be off the line (order -1)
      //          be LATER than any other NOT INCLUDED (so order != 1) yet valid (order > -1) candidate
      if (included[i] == -1 || included[i] == 1){
        continue;
      }
      bool foundOtherCandidate = false;
      for (int j=0; j<MAX_N_TRAPS; j++){
        if (i == j || included[j] == -2 || included[j] == 1){
          continue;
        }
        /* if (isFirstPointSooner(trapX[j],trapY[j],trapX[i],trapY[i])){ */
        if (isFirstPointLower(trapX[j],trapY[j],trapX[i],trapY[i])){
          foundOtherCandidate = true;
          break;
        }
      }
      if (foundOtherCandidate){
        break;
      }
      trapD[nIncluded++]=horizontal?trapX[i]:trapY[i];
      included[i] = 1;
    }
  }
  // assume horizontal TODO update
  int nTrapsUsedInASubline = 0;
  int nSublines = 0;
  float da;
  float db;
  if (horizontal){
    da = xa;
    db = xb;
  }
  else{
    da = ya;
    db = yb;
  }
  while(true){
    float d1;
    float d4;
    float v1;
    float v4;
    // assign left
    if (nSublines == 0){
      if (orientation == 1){
        d1 = da;
        v1 = CURVE_SPEED;
      }
      else{
        d4 = da;
        v4 = CURVE_SPEED;
      }
    }
    else{
      if (orientation == 1){
        d1 = sublines[nSublines].d4;
        v1 = sublines[nSublines].v4;
      }
      else{
        d4 = sublines[nSublines].d4;
        v4 = sublines[nSublines].v4;
      }
    }

    // assign right
    if (nTraps-nTrapsUsedInASubline == 0){ // should end with main line seg ending
      if (orientation == 1){
        d4 = db;
        v4 = CURVE_SPEED;
      }
      else{
        d1 = db;
        v1 = CURVE_SPEED;
      }
    }
    else{ // create a subline ending w/ start of next trap followed by a trap subline
      if (orientation == 1){
        // trapD is a distance along its axis in units of tiles
        d4 = trapD[nTrapsUsedInASubline]+0.5;
        v4 = TRAP_SPEED;
      }
      else {
        d1 = trapD[nTrapsUsedInASubline]-0.5;
        v1 = TRAP_SPEED;
      }
      sublines[nSublines++] = Subline(d1, d4, v1, v4);
      nTrapsUsedInASubline++;
      if (orientation == 1){
        d1 = d4;
        d4 = d1+1;
        v1 = TRAP_SPEED;
        // 0.0001 is for float equality checking
        if (d4+EPSILON >= db){ // check if the trap ends at the end of the line segment
          // in which case we want to end the trap segment @ curve speed and then break 
          v4 = CURVE_SPEED;
        }
        else{
          v4 = TRAP_SPEED;
        }
      }
      else{ // orientation == -1
        d4 = d1;
        d1 = d4-1;
        v4 = TRAP_SPEED;
        if (d1-EPSILON <= db){ // check if the trap ends at the end of the line segment
          // in which case we want to end the trap segment @ curve speed and then break 
          v1 = CURVE_SPEED;
        }
        else{
          v1 = TRAP_SPEED;
        }
      }
    }
    sublines[nSublines++] = Subline(d1, d4, v1, v4);
    if (orientation == 1){
      if (sublines[nSublines].d4 >= db+EPSILON){
        break;
      }
    }
    else{
      if (sublines[nSublines].d1 <= db-EPSILON){
        break;
      }
    }
  }
}

SegmentType Line::getType(){
  return LINE;
}

// takes into account orientation
bool Line::isFirstPointSooner(float xp1, float yp1, float xp2, float yp2){
  float d1;
  float d2;
  if (horizontal){
    d2 = xp2;
    d1 = xp1;
  }
  else{
    d2 = yp2;
    d1 = yp1;
  }
  return (d2>d1&&(orientation==1))||(d2<d1&&(orientation==-1));
}

// does not take into account orientation
bool Line::isFirstPointLower(float xp1, float yp1, float xp2, float yp2){
  float d1;
  float d2;
  if (horizontal){
    d2 = xp2;
    d1 = xp1;
  }
  else{
    d2 = yp2;
    d1 = yp1;
  }
  return d2>d1;
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
    return xp-xa;
  }
  else{
    return yp-ya;
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

Curve::Curve(float xc, float yc, CornerType cornerType):
  xc(xc), // center x
  yc(yc), // center y
  cornerType(cornerType)
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
  segments[0] = new Line(3.5,5.5,1,5.5,cmdData->trapX,cmdData->trapY);
  segments[1] = new Curve(1,5,BL);
  segments[2] = new Line(0.5,5,0.5,1,cmdData->trapX,cmdData->trapY);
  segments[3] = new Curve(1,1,TL);
  segments[4] = new Line(1,0.5,5,0.5,cmdData->trapX,cmdData->trapY);
  segments[5] = new Curve(5,1,TR);
  segments[6] = new Line(5.5,1,5.5,4,cmdData->trapX,cmdData->trapY);
  segments[7] = new Curve(5,4,BR_);
  segments[8] = new Line(5,4.5,2,4.5,cmdData->trapX,cmdData->trapY);
  segments[9] = new Curve(2,4,BL);
  segments[10] = new Line(1.5,4,1.5,2,cmdData->trapX,cmdData->trapY);
  segments[11] = new Curve(2,2,TL);
  segments[12] = new Line(2,1.5,4,1.5,cmdData->trapX,cmdData->trapY);
  segments[13] = new Curve(4,2,TR);
  segments[14] = new Line(4.5,2,4.5,3,cmdData->trapX,cmdData->trapY);
  segments[15] = new Curve(4,3,BR_);
  segments[16] = new Line(4,3.5,3,3.5,cmdData->trapX,cmdData->trapY);
  segments[17] = new Curve(3,3,BL);
  segments[18] = new Curve(3,3,TL);
  segments[19] = new Line(3,2.5,3.5,2.5,cmdData->trapX,cmdData->trapY);
}

// returns true if cmdData contanis new traps
bool Traj::trapsChanged(){
  return cmdData->nTraps != nTraps;
}

void Traj::updateTraps(){
  /* for (int i=0; i<NUM_SEGMENTS; i++){ */
    /* if (segments[i]->getType() == LINE){ */
      /* *segments[i] = copyAndRecalculateTraps(static_cast<Line*>(segments[i]), cmdData->trapX, cmdData->trapY); */
    /* } */
  /* } */
}

// returns true if finished driving the track, false otherwise
bool Traj::updatePos(float xp, float yp){
  if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updatePos");
  bool advanced = false;
  while(segments[gd->segNum]->completed(xp, yp)){
    if (hms->data.guidanceLogLevel >= 2) Serial.println("blah1");
    if (advanced){
      if (hms->data.guidanceLogLevel >= 2) Serial.println("blah2");
      hms->logError(HmsData_Error_WTF_AHMAD, "skipped track segment!");
    }
    if (hms->data.guidanceLogLevel >= 2) Serial.println("blah3");
    if (gd->segNum++ >= NUM_SEGMENTS){
      Serial.println("Completed track!");
      gd->segNum--;
      return true;
    }
    advanced = true;
  }
  return false;
}

float Traj::getDist(float xp, float yp){
  return segments[gd->segNum]->getDist(xp,yp);
}

float Traj::getSetpointVel(float xp, float yp){
  return segments[gd->segNum]->velSetpoint(xp, yp);
}
