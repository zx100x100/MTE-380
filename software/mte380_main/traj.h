#ifndef TRAJ_H
#define TRAJ_H

#include "hms_and_cmd_data.pb.h"
#include "hms.h"
#include "guidance_data.pb.h"

#define NUM_SEGMENTS 20
#define MAX_N_TRAPS 8

enum CornerType { TL, TR, BL, BR_ };
enum SegmentType { CURVE, LINE };

class Subline{
  public:
    Subline();
    Subline(float,float,float,float);
    float trapezoidal(float);
    bool isDOnLine(float);
    float d1;
    float d2;
    float d3;
    float d4;
    float dt;
    float v1;
    float v2;
    float v3;
    float v4;
};

class Segment {
  public:
    virtual float getDist(float, float);
    virtual bool completed(float, float);
    virtual float velSetpoint(float, float);
    virtual SegmentType getType();

  // protected:
    // completed(float x, float y);
};

class Curve: public Segment {
  public:
    Curve(float,float,CornerType);
    float getDist(float, float);
    bool completed(float, float);
    float velSetpoint(float, float);
    SegmentType getType();

  private:
    float xc;
    float yc;
    CornerType cornerType;
};

class Line: public Segment {
  public:
    Line(float,float,float,float,float[MAX_N_TRAPS],float[MAX_N_TRAPS]);
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

  private:
    int orientation;
    bool horizontal;
    int nSublines;
    int nTraps;
    Subline sublines[MAX_N_TRAPS];
    float trapD[MAX_N_TRAPS];
};

class Traj{
  public:
    Traj();
    Traj(Hms* hms, GuidanceData* gd, CmdData* cmdData);
    bool updatePos(float xp, float yp);
    bool trapsChanged();
    void updateTraps();
    float getDist(float xp, float yp);
    float getSetpointVel(float xp, float yp);

  private:
    Segment* segments[NUM_SEGMENTS];
    int nTraps;
    Hms* hms;
    GuidanceData* gd;
    CmdData* cmdData;
};

#endif
