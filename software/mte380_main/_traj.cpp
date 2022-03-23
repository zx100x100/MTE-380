// #include "traj.h"
// #include "Arduino.h"

// #define CURVE_RADIUS 0.5 // tiles
// #define CURVE_SPEED 1 // tiles/s
// #define ACC 12 // tiles/s^2
// #define VMAX 3 // tiles/s^2
// #define TRAP_SPEED 2 // tiles/s
// #define EPSILON 0.000001 // for float equality

// [> Line::Line(float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS], Hms* hms){ <]
// [> } <]

// [> Line copyAndRecalculateTraps(Line* line, float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS], Hms* hms){ <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.println("copyAndRecalculateTraps"); } <]
  // [> return Line(line->xa, line->ya, line->xb, line->yb, trapX, trapY, line->hms); <]
// [> } <]


// Subline::Subline(){}
// Subline::Subline(float d1, float d4, float v1, float v4, Hms* hms):
  // d1(d1),
  // d4(d4),
  // v1(v1),
  // v4(v4),
  // hms(hms)
// {
  // d2 = d1 + (pow(VMAX,2) - pow(v1,2))/(2*ACC);// - d1;
  // d3 = d4 - (pow(VMAX,2)-pow(v4,2))/(2*ACC);
  // dt = 0.5*((pow(v4,2)-pow(v1,2))/(2*ACC) + d1 + d4);
// }

// float Subline::trapezoidal(float d){
  // // Serial.println("trapezoidal");
  // if(hms->data.guidanceLogLevel >= 2){ Serial.println("trapezoidal"); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d1: "); Serial.println(d1); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d2: "); Serial.println(d2); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d3: "); Serial.println(d3); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d4: "); Serial.println(d4); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("dt: "); Serial.println(dt); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("v1: "); Serial.println(v1); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("v4: "); Serial.println(v4); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d: "); Serial.println(d); }
  // if (d3 < d2){
    // if (dt < d1){
      // if(hms->data.guidanceLogLevel >= 2){ Serial.println("dt < d1"); }
      // if(hms->data.guidanceLogLevel >= 2){ Serial.print("v1+(v4-v1)*(d-d1)/(d4-d1): "); Serial.println(v1+(v4-v1)*(d-d1)/(d4-d1)); }
      // return v1+(v4-v1)*(d-d1)/(d4-d1);
    // }
    // if (d < dt){
      // if(hms->data.guidanceLogLevel >= 2){ Serial.println("d < dt"); }
      // if(hms->data.guidanceLogLevel >= 2){ Serial.print("pow(2*ACC*(d-d1)+pow(v1,2),0.5): "); Serial.println(pow(2*ACC*(d-d1)+pow(v1,2),0.5)); }
      // return pow(2*ACC*(d-d1)+pow(v1,2),0.5);
    // }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.println("d > dt"); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("pow(pow(v4,2)+2*ACC*(d4-d),0.5): "); Serial.println(pow(pow(v4,2)+2*ACC*(d4-d),0.5)); }
    // return pow(pow(v4,2)+2*ACC*(d4-d),0.5);
  // }
  // if (d<d2){
    // if(hms->data.guidanceLogLevel >= 2){ Serial.println("d<d2"); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("pow(2*ACC*(d-d1)+pow(v1,2),0.5): "); Serial.println(pow(2*ACC*(d-d1)+pow(v1,2),0.5)); }
    // return pow(2*ACC*(d-d1)+pow(v1,2),0.5);
  // }
  // if (d<d3){
    // if(hms->data.guidanceLogLevel >= 2){ Serial.println("d<d3"); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("VMAX: "); Serial.println(VMAX); }
    // return VMAX;
  // }
  // return pow(pow(v4,2)+2*ACC*(d4-d),0.5);
// }


// bool Subline::isDOnLine(float d){
  // [> Serial.print("d1: "); Serial.println(d1); <]
  // [> Serial.print("d4: "); Serial.println(d4); <]
  // return d < d4 && d >= d1;
// }

// float Line::velSetpoint(float xp, float yp){
  // float dp;
  // if (horizontal){
    // dp = xp;
  // }
  // else{
    // dp = yp;
  // }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.println("Line::velSetpoint"); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("nSublines: "); Serial.println(nSublines); }
  // for (int i=0; i<nSublines; i++){
    // if(hms->data.guidanceLogLevel >= 2){ Serial.println("loop?"); }
    // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("dp: "); Serial.println(dp); } <]
    // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[i].isDOnLine(dp): "); Serial.println(sublines[i].isDOnLine(dp)); } <]
    // if (sublines[i].isDOnLine(dp+EPSILON) || sublines[i].isDOnLine(dp-EPSILON)){
      // if (orientation == -1 && false){
        // // if the line segment goes bottom to top or right to left, that means we reversed our d1 and d4
        // // which means that we need to mirror our d across the center of the line segment.
        // [> float fromStart = dp - sublines[i].d1;  <]
        // float fromStart = sublines[i].d4 - dp; 
        // [> now make d be the same distance from d4 as it was from d1: <]
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("fromStart: "); Serial.println(fromStart); }
        // dp = sublines[i].d1 + fromStart;
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("dp: "); Serial.println(dp); }
        // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("TRUEdp: "); Serial.println(dp); } <]
      // }
      // if(hms->data.guidanceLogLevel >= 2){ Serial.print("dp: "); Serial.println(dp); }
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("BLAHHHHHHHHH nTraps: "); Serial.println(nTraps); } <]
      // return sublines[i].trapezoidal(dp);
    // }
  // }
  // // raise HMS error????????? TODO
  // return 0;
// }

// Line::Line(float _xa, float _ya, float _xb, float _yb, float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS], Hms* hms):
  // hms(hms)
// {
  // if(hms->data.guidanceLogLevel >= 2){ Serial.println("Line::init"); }
  // horizontal = _ya == _yb;
  // if (horizontal){
    // orientation = _xa > _xb ? -1 : 1;
  // }
  // else{
    // orientation = _ya > _yb ? -1 : 1;
  // }
  // if (horizontal){
    // xa = _xa+BULLSHIT*orientation==-1?0.5:-0.5;
    // xb = _xb-BULLSHIT*orientation==-1?0.5:-0.5;
    // ya = _ya;
    // yb = _yb;
  // }
  // else{
    // ya = _ya+BULLSHIT*orientation==-1?0.5:-0.5;
    // yb = _yb-BULLSHIT*orientation==-1?0.5:-0.5;
    // xa = _xa;
    // xb = _xb;
  // }
  
  // float included[MAX_N_TRAPS];
  // nTraps = 0;

  // for (int i=0; i<MAX_N_TRAPS; i++){
    // if (isPointOnLine(trapX[i], trapY[i])){
      // if(hms->data.guidanceLogLevel >= 2){ Serial.print("Trap "); Serial.print(i); Serial.println(" found on line");}
      // included[i] = 0;
      // nTraps++;
    // }
    // else{
      // included[i] = -1;
    // }
  // }
  // int nIncluded = 0;
  // int nIterations = 0;
  // int MAX_ITERATIONS = 100;
  // if(hms->data.guidanceLogLevel >= 2){ Serial.println("begin line init trap loop"); }
  // while(nIncluded < nTraps){
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("nIncluded: "); Serial.println(nIncluded); }
    // if(nIterations++ > MAX_ITERATIONS){
      // Serial.println("LOOP DIDNT EXIT, FIX UR SHIT KAELAN!!!!!!!!");
      // while(true){
        // sleep(100);
      // }
      // break;
    // }
    // for (int i=0; i<MAX_N_TRAPS; i++){
      // if(hms->data.guidanceLogLevel >= 2){ Serial.print("starting candidate i="); Serial.println(i); }
      // // candidate i.
      // // must be: not already in array (order 1)
      // //          be off the line (order -1)
      // //          be LATER than any other NOT INCLUDED (so order != 1) yet valid (order > -1) candidate
      // if (included[i] == -1 || included[i] == 1){
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("skipping, included[i]: "); Serial.println(included[i]); }
        // continue;
      // }
      // bool foundOtherCandidate = false;
      // for (int j=0; j<MAX_N_TRAPS; j++){
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("starting alt-candidate j="); Serial.println(j); }
        // if (i == j || included[j] == -1 || included[j] == 1){
          // if(hms->data.guidanceLogLevel >= 2){ Serial.println("skipping"); }
          // continue;
        // }
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("included[j]: "); Serial.println(included[j]); }
        // [> if (isFirstPointSooner(trapX[j],trapY[j],trapX[i],trapY[i])){ <]
        // if (isFirstPointLower(trapX[j],trapY[j],trapX[i],trapY[i])){
          // if(hms->data.guidanceLogLevel >= 2){ Serial.print("trap j="); Serial.print(j); Serial.print(" was encountered sooner than i="); Serial.print(i); Serial.println(", breaking");}
          // foundOtherCandidate = true;
          // break;
        // }
      // }
      // if (foundOtherCandidate){
        // if(hms->data.guidanceLogLevel >= 2){ Serial.println("foundOtherCandidate=true, continuing outer candidate loop"); }
        // continue;
      // }
      // if(hms->data.guidanceLogLevel >= 2){ Serial.println("setting trapD thing, incrementing nIncluded"); }
      // trapD[nIncluded++]=horizontal?trapX[i]:trapY[i];
      // included[i] = 1;
    // }
  // }

  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("nIncluded: "); Serial.println(nIncluded); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("trapD[0]: "); Serial.println(trapD[0]); }

  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.println("begin subline creation"); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("nTraps: "); Serial.println(nTraps); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("orientation: "); Serial.println(orientation); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("horizontal: "); Serial.println(horizontal); } <]
  // int nTrapsUsedInASubline = 0;
  // int lastSubline = -1;
  // float da;
  // float db;
  // if (horizontal){
    // da = xa;
    // db = xb;
  // }
  // else{
    // da = ya;
    // db = yb;
  // }
  // while(true){
    // float d1;
    // float d4;
    // float v1;
    // float v4;
    // // assign left
    // if (lastSubline == -1){
      // if (orientation == 1){
        // d1 = da;
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d1: "); Serial.println(d1); }
        // v1 = CURVE_SPEED;
      // }
      // else{
        // d4 = da;
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("d4: "); Serial.println(d4); }
        // if(da<3.5+EPSILON && da>3.5-EPSILON){ // if this is the starting line segment, start with v=0
          // v4 = 0;
        // }
        // else{
          // v4 = CURVE_SPEED;
        // }
      // }
    // }
    // else{
      // if (orientation == 1){
        // d1 = sublines[lastSubline].d4;
        // v1 = sublines[lastSubline].v4;
      // }
      // else{
        // if(hms->data.guidanceLogLevel >= 2){ Serial.println("Should be assigning d4=2"); }
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("lastSubline: "); Serial.println(lastSubline); }
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[lastSubline]: "); Serial.println(sublines[lastSubline].d1); }
        // d4 = sublines[lastSubline].d1;
        // v4 = sublines[lastSubline].v1;
      // }
    // }

    // // assign right
    // if (nTraps-nTrapsUsedInASubline == 0){ // should end with main line seg ending
      // if (orientation == 1){
        // d4 = db;
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("ending w main line seg: d4: "); Serial.println(d4); }
        // v4 = CURVE_SPEED;
      // }
      // else{ // ends with next trap
        // d1 = db;
        // if(hms->data.guidanceLogLevel >= 2){ Serial.print("ending w main line seg: d1: "); Serial.println(d1); }
        // v1 = CURVE_SPEED;
      // }
    // }
    // else{ // create a subline ending w/ start of next trap followed by a trap subline
      // bool skipPreTrapSubline = false; // skip if this is the 0th subline and also a trap
      // if (orientation == 1){
        // if (trapD[nTrapsUsedInASubline] - 0.5 == d1){
          // skipPreTrapSubline = true;
        // }
      // }
      // else{
        // if (trapD[nTrapsUsedInASubline] + 0.5 == d4){
          // skipPreTrapSubline = true;
        // }
      // }
      // if (!skipPreTrapSubline){
        // if (orientation == 1){
          // // trapD is a distance along its axis in units of tiles
          // d4 = trapD[nTrapsUsedInASubline]-0.5;
          // v4 = TRAP_SPEED;
        // }
        // else {
          // if(hms->data.guidanceLogLevel >= 2){ Serial.print("trapD[nTrapsUsedInASubline]: "); Serial.println(trapD[nTrapsUsedInASubline]); }
          // d1 = trapD[nTrapsUsedInASubline]+0.5;
          // v1 = TRAP_SPEED;
        // }
        // if (lastSubline > MAX_N_TRAPS-1){
          // Serial.println("TOO MANY SUBLINES!!!!!!");
          // return;
        // }
        // sublines[++lastSubline] = Subline(d1, d4, v1, v4, hms);
        // nTrapsUsedInASubline++;
      // }
      // // set up trap subline
      // if (orientation == 1){
        // d1 = d4;
        // d4 = d1+1;
        // v1 = TRAP_SPEED;
        // // 0.0001 is for float equality checking
        // if (d4+EPSILON >= db){ // check if the trap ends at the end of the line segment
          // // in which case we want to end the trap segment @ curve speed and then break 
          // v4 = CURVE_SPEED;
        // }
        // else{
          // v4 = TRAP_SPEED;
        // }
      // }
      // else{ // orientation == -1
        // d4 = d1;
        // d1 = d4-1;
        // v4 = TRAP_SPEED;
        // if (d1-EPSILON <= db){ // check if the trap ends at the end of the line segment
          // // in which case we want to end the trap segment @ curve speed and then break 
          // v1 = CURVE_SPEED;
        // }
        // else{
          // v1 = TRAP_SPEED;
        // }
      // }
    // }
    // if (lastSubline > MAX_N_TRAPS-1){
      // Serial.println("TOO MANY SUBLINES!!!!!!");
      // return;
    // }
    // else{
      // if(hms->data.guidanceLogLevel >= 2){ Serial.print("x lastSubline: "); Serial.println(lastSubline); }
    // }
    // sublines[++lastSubline] = Subline(d1, d4, v1, v4, hms);
    // if (orientation == 1){
      // if(hms->data.guidanceLogLevel >= 2){ Serial.println("stuff123"); }
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[lastSubline].d4+EPSILON: "); Serial.println(sublines[lastSubline].d4 + EPSILON); } <]
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("db: "); Serial.println(db); } <]
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[lastSubline].d4+EPSILON >= db: "); Serial.println(sublines[lastSubline].d4+EPSILON >= db); } <]
      // heap_caps_check_integrity(MALLOC_CAP_8BIT, true);
      // if (sublines[lastSubline].d4+EPSILON >= db){
        // break;
      // }
    // }
    // else{
      // if(hms->data.guidanceLogLevel >= 2){ Serial.println("stuff789"); }
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[lastSubline].d1-EPSILON: "); Serial.println(sublines[lastSubline].d1-EPSILON); } <]
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("db: "); Serial.println(db); } <]
      // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[lastSubline].d1-EPSILON <= db: "); Serial.println(sublines[lastSubline].d1-EPSILON <= db); } <]
      // if (sublines[lastSubline].d1-EPSILON <= db){
        // if(hms->data.guidanceLogLevel >= 2){ Serial.println("break????"); }
        // [> if(hms->data.guidanceLogLevel >= 2){ Serial.println("breaking"); } <]
        // break;
      // }
    // }
  // }
  // nSublines = lastSubline + 1;
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("nSublines: "); Serial.println(nSublines); }

  // heap_caps_check_integrity(MALLOC_CAP_8BIT, true);
  // if (nSublines > 1){
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].d1: "); Serial.println(sublines[0].d1); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].d2: "); Serial.println(sublines[0].d2); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].d3: "); Serial.println(sublines[0].d3); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].d4: "); Serial.println(sublines[0].d4); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].v1: "); Serial.println(sublines[0].v1); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].v2: "); Serial.println(sublines[0].v2); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].v3: "); Serial.println(sublines[0].v3); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[0].v4: "); Serial.println(sublines[0].v4); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].d1: "); Serial.println(sublines[1].d1); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].d2: "); Serial.println(sublines[1].d2); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].d3: "); Serial.println(sublines[1].d3); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].d4: "); Serial.println(sublines[1].d4); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].v1: "); Serial.println(sublines[1].v1); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].v2: "); Serial.println(sublines[1].v2); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].v3: "); Serial.println(sublines[1].v3); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[1].v4: "); Serial.println(sublines[1].v4); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].d1: "); Serial.println(sublines[2].d1); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].d2: "); Serial.println(sublines[2].d2); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].d3: "); Serial.println(sublines[2].d3); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].d4: "); Serial.println(sublines[2].d4); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].v1: "); Serial.println(sublines[2].v1); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].v2: "); Serial.println(sublines[2].v2); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].v3: "); Serial.println(sublines[2].v3); }
    // if(hms->data.guidanceLogLevel >= 2){ Serial.print("sublines[2].v4: "); Serial.println(sublines[2].v4); }
  // }
// }

// SegmentType Line::getType(){
  // return LINE;
// }

// // takes into account orientation
// bool Line::isFirstPointSooner(float xp1, float yp1, float xp2, float yp2){
  // float d1;
  // float d2;
  // if (horizontal){
    // d2 = xp2;
    // d1 = xp1;
  // }
  // else{
    // d2 = yp2;
    // d1 = yp1;
  // }
  // return (d2>d1&&(orientation==1))||(d2<d1&&(orientation==-1));
// }

// // does not take into account orientation
// bool Line::isFirstPointLower(float xp1, float yp1, float xp2, float yp2){
  // float d1;
  // float d2;
  // if (horizontal){
    // d2 = xp2;
    // d1 = xp1;
  // }
  // else{
    // d2 = yp2;
    // d1 = yp1;
  // }
  // return d2>d1;
// }

// bool Line::isPointOnLine(float xp, float yp){
  // if (horizontal){
    // if (yp == ya){
      // return min(xa,xb) < xp && max(xa,xb) > xp;
    // }
    // return false;
  // }
  // if (xp == xa){
    // return min(ya,yb) < yp && max(ya,yb) > xp;
  // }
  // return false;
// }

// float Line::getDist(float xp, float yp){
  // if (horizontal){
    // if (orientation == -1){
      // return yp - ya;
    // }
    // else{
      // return ya - yp;
    // }
  // }
  // else{
    // if (orientation == -1){
      // return xa - xp;
    // }
    // else{
      // return xp - xa;
    // }
  // }
// }

// bool Line::completed(float xp, float yp){
  // if (horizontal){
    // if (xb > xa){
      // return xp > xb;
    // }
    // return xp < xb;
  // }
  // if (yb > ya){
    // return yp > yb;
  // }
  // return yp < yb;
// }

// Curve::Curve(float xc, float yc, CornerType cornerType, Hms* hms):
  // xc(xc), // center x
  // yc(yc), // center y
  // cornerType(cornerType),
  // hms(hms)
// {
// }

// SegmentType Curve::getType(){
  // return CURVE;
// }

// float Curve::velSetpoint(float xp, float yp){
  // return CURVE_SPEED;
// }

// // returns negative if inside circle, positive if outside circle
// float Curve::getDist(float xp, float yp){
  // return pow(pow(xp-xc,2)+pow(yp-yc,2),0.5) - CURVE_RADIUS;
// }

// bool Curve::completed(float xp, float yp){
  // if (cornerType == BL){
    // return yp < yc;
  // }
  // else if (cornerType == BR_){
    // return xp < xc;
  // }
  // else if (cornerType == TL){
    // return xp > xc;
  // }
  // else{
    // return yp > yc;
  // }
// }

// Traj::Traj(){}
// Traj::Traj(Hms* hms, GuidanceData* gd, CmdData* cmdData):
  // hms(hms),
  // gd(gd),
  // cmdData(cmdData)
// {
  // nTraps = cmdData->nTraps;
  // gd->segNum = 0;
// }

// void Traj::init(){
  // [> Traj::init() <]
  // // THIS PART OF THE CODE HAS NEVER (NOT EVEN ONCE) HAD HEAP CORRUPTION AKA POISONING ISSUES!!!
  // // BUT IF IT EVER DOES, MY FRIEND SAID THAT UNCOMMENTING THIS WOULD BE HELPFUL...

  // [> heap_caps_check_integrity(MALLOC_CAP_8BIT, true); <]

  // [> segments[0] = new Line(3.5,5.5,1,5.5,cmdData->trapX,cmdData->trapY,hms); <]
  // segments[0] = new Line(3.5,5.5,1,5.5,cmdData->trapX,cmdData->trapY,hms);
  // segments[1] = new Curve(1,5,BL,hms);
  // segments[2] = new Line(0.5,5,0.5,1,cmdData->trapX,cmdData->trapY,hms);
  // segments[3] = new Curve(1,1,TL,hms);
  // segments[4] = new Line(1,0.5,5,0.5,cmdData->trapX,cmdData->trapY,hms);
  // segments[5] = new Curve(5,1,TR,hms);
  // segments[6] = new Line(5.5,1,5.5,4,cmdData->trapX,cmdData->trapY,hms);
  // segments[7] = new Curve(5,4,BR_,hms);
  // segments[8] = new Line(5,4.5,2,4.5,cmdData->trapX,cmdData->trapY,hms);
  // segments[9] = new Curve(2,4,BL,hms);
  // segments[10] = new Line(1.5,4,1.5,2,cmdData->trapX,cmdData->trapY,hms);
  // segments[11] = new Curve(2,2,TL,hms);
  // segments[12] = new Line(2,1.5,4,1.5,cmdData->trapX,cmdData->trapY,hms);
  // segments[13] = new Curve(4,2,TR,hms);
  // segments[14] = new Line(4.5,2,4.5,3,cmdData->trapX,cmdData->trapY,hms);
  // segments[15] = new Curve(4,3,BR_,hms);
  // segments[16] = new Line(4,3.5,3,3.5,cmdData->trapX,cmdData->trapY,hms);
  // segments[17] = new Curve(3,3,BL,hms);
  // segments[18] = new Curve(3,3,TL,hms);
  // segments[19] = new Line(3,2.5,3.5,2.5,cmdData->trapX,cmdData->trapY,hms);
// }

// // returns true if cmdData contanis new traps
// bool Traj::trapsChanged(){
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("cmdData-nTraps: "); Serial.println(cmdData->nTraps); }
  // if(hms->data.guidanceLogLevel >= 2){ Serial.print("nTraps: "); Serial.println(nTraps); }
  // bool changed = cmdData->nTraps != nTraps;
  // nTraps = cmdData->nTraps;
  // return changed;
// }

// void Traj::updateTraps(){
  // if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updateTraps()");
  // for (int i=0; NUM_SEGMENTS; i++){
    // if (segments[i]->getType() == LINE){
      // [> *segments[i] = copyAndRecalculateTraps(static_cast<Line*>(segments[i]), cmdData->trapX, cmdData->trapY, hms); <]
  // [> return Line(line->xa, line->ya, line->xb, line->yb, trapX, trapY, line->hms); <]
      // Line* tempLine = static_cast<Line*>(segments[i]);
      // Line* newLine = new Line(tempLine->xa, tempLine->ya, tempLine->xb, tempLine->yb, cmdData->trapX, cmdData->trapY, hms);
      // delete segments[i];
      // segments[i] = newLine;
    // }
  // }
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("segments[0]->nSublines: "); Serial.println(segments[0]->nSublines); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("segments[0]->sublines[0].d1;: "); Serial.println(segments[0]->sublines[0].d1); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("segments[0]->sublines[0].v1;: "); Serial.println(segments[0]->sublines[0].v1); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("segments[0]->sublines[0].d4;: "); Serial.println(segments[0]->sublines[0].d4); } <]
  // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("segments[0]->sublines[0].v4;: "); Serial.println(segments[0]->sublines[0].v4); } <]
// }

// // returns true if finished driving the track, false otherwise
// bool Traj::updatePos(float xp, float yp){

  // if (hms->data.guidanceLogLevel >= 2) Serial.println("Traj::updatePos");
  // bool advanced = false;

  // while(segments[gd->segNum]->completed(xp, yp)){
    // [> if(hms->data.guidanceLogLevel >= 2){ Serial.print("gd->segNum: "); Serial.println(gd->segNum); } <]
    // [> if (hms->data.guidanceLogLevel >= 2) Serial.println("blah1"); <]
    // if (advanced){
      // [> if (hms->data.guidanceLogLevel >= 2) Serial.println("blah2"); <]
      // [> hms->logError(HmsData_Error_WTF_AHMAD, "skipped track segment!"); <]
    // }
    // [> if (hms->data.guidanceLogLevel >= 2) Serial.println("blah3"); <]
    // if (gd->segNum++ >= NUM_SEGMENTS){
      // Serial.println("Completed track!");
      // gd->segNum--;
      // return true;
    // }
    // advanced = true;
  // }
  // [> if (hms->data.guidanceLogLevel >= 2) Serial.println("done updatePos"); <]
  // return false;
// }

// float Traj::getDist(float xp, float yp){
  // return segments[gd->segNum]->getDist(xp,yp);
// }

// float Traj::getSetpointVel(float xp, float yp){
  // if(hms->data.guidanceLogLevel >= 2){ Serial.println("Traj::getSetpointVel"); }
  // return segments[gd->segNum]->velSetpoint(xp, yp);
// }
