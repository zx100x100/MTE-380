#ifndef SORRY_H
#define SORRY_H
#include "nav.h"
#include "motors.h"
#include "sensors.h"
#include "hms.h"

class Sorry{
  public:
    Sorry();
    Sorry(Motors* motors, Sensors* sensors, Nav* nav, Hms* hms);

    bool isValid(int tofNum);
    float getTofFt(int tofNum);
    bool updateWallAngleAndDistance();
    void driveTick(float motorPower, float leftWallDist);
    void drive(unsigned long goMediumForFirst, unsigned long goFastFor, unsigned long goFastUnguidedDur, unsigned long goMediumFor, float distanceToStopAt, float leftWallDist, bool ultraPower=false);
    void turnInPlace();
    void run();

    float errDriftI;
    float errDrift;
    float left;
    float angFromWall;

    unsigned long curT;
    unsigned long deltaT;

    bool dontCorrectDrift;

    Hms* hms;
    Nav* nav;
    Motors* motors;
    Sensors* sensors;

};
#endif
