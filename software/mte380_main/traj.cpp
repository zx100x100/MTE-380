#include "traj.h"
#include "Arduino.h"

enum Corner { TL, TR, BL, BR };

Line::Line(bool active):
  active(active)
{
  
}

float Line::getDist(){
  return 7.0;
}

Curve::Curve(){
}

float Curve::getDist(){
  return 3.0;
}

Traj::Traj()
{
  segments[0] = new Line(3.5,5.5,1,5.5,true);
  segments[1] = new Curve(1,5,BL);
  segments[2] = new Line(0.5,5,0.5,1);
  segments[3] = new Curve(1,1,TL);
  segments[4] = new Line(1,0.5,5,0.5);
  segments[5] = new Curve(5,1,TR);
  segments[6] = new Line(5.5,1,5.5,4);
  segments[7] = new Curve(5,4,BR);
  segments[8] = new Line(5,4.5,2,4.5);
  segments[9] = new Curve(2,4,BL);
  segments[10] = new Line(1.5,4,1.5,2);
  segments[11] = new Curve(2,2,TL);
  segments[12] = new Line(2,1.5,4,1.5);
  segments[13] = new Curve(4,2,TR);
  segments[14] = new Line(4.5,2,4.5,3);
  segments[15] = new Curve(4,3,BR);
  segments[16] = new Line(4,3.5,3,3.5);
  segments[17] = new Curve(3,3,BL);
  segments[18] = new Curve(3,3,TL);
  segments[19] = new Line(3,2.5,3.5,2.5);













}
