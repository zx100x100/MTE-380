#ifndef TRAJ_H
#define TRAJ_H

#include "hms_and_cmd_data.pb.h"
#include "hms.h"
#include "guidance_data.pb.h"

#define NUM_SEGMENTS 20
#define MAX_N_TRAPS 8
#define CORNER_OFFSET_BULLSHIT_FOR_TURN_IN_PLACE 0.2
//0.5


enum CornerType { TL, TR, BL, BR_ }; // BR was taken by something built in lol
enum SegmentType { CURVE, LINE };


class Subline{
  public:
    Subline();
    Subline(float,float,float,float,Hms*);
    float trapezoidalAcceleration(float);
    bool isDOnLine(float d, int endCondition=0);
    float d1;
    float d2;
    float d3;
    float d4;
    float v1;
    float v4;
    float vm;
    float a;

  private:
    Hms* hms;
};

class Segment {
  public:
    virtual float getDist(float xp, float yp);
    virtual bool completed(float xp, float yp);
    virtual float velSetpoint(float, float);
    virtual SegmentType getType();
  // protected:
    // completed(float x, float y);
};

class Curve: public Segment {
  public:
    Curve(float,float,CornerType, Hms* hms);
    float getDist(float, float);
    bool completed(float, float);
    float velSetpoint(float, float);
    SegmentType getType();

  private:
    Hms* hms;
    float xc;
    float yc;
    CornerType cornerType;
};

class Line: public Segment {
  public:
    Line(float,float,float,float,float[MAX_N_TRAPS],float[MAX_N_TRAPS], Hms* hms);
    Line(Line* line);
    float getDist(float, float);
    bool completed(float, float);
    float velSetpoint(float, float);
    bool isPointOnLine(float, float);
    bool isFirstPointSooner(float,float,float,float);
    bool isFirstPointLower(float,float,float,float);
    SegmentType getType();
    float xa;
    float ya;
    float xb;
    float yb;
    Hms* hms;
    int nSublines; // temp, make private again
    int orientation;
    bool horizontal;
    void updateTraps(float trapX[MAX_N_TRAPS], float trapY[MAX_N_TRAPS]);
    Subline sublines[MAX_N_TRAPS];

  private:
    float trapD[MAX_N_TRAPS];
};

class Traj{
  public:
    Traj();
    Traj(Hms* hms, GuidanceData* gd, CmdData* cmdData);
    void init();
    bool updatePos(float xp, float yp);
    bool trapsChanged();
    void updateTraps();
    float getDist(float xp, float yp);
    float getSetpointVel(float xp, float yp);
    Segment* segments[NUM_SEGMENTS];

  private:
    int nTraps;
    Hms* hms;
    GuidanceData* gd;
    CmdData* cmdData;
};

#endif
