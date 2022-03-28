#ifndef SORRY_H
#define SORRY_H
#include "nav.h"
#include "motors.h"
#include "sensors.h"
#include "hms.h"

enum CorrectionMode{ UNGUIDED, GUIDED, PARALLEL, GUIDED_GYRO };

class Sorry{
  public:
    Sorry();
    Sorry(Motors* motors, Sensors* sensors, Nav* nav, Hms* hms);

    bool isValid(int tofNum);
    float getTofFt(int tofNum);
    bool updateWallAngleAndDistance();
    void driveTick(float motorPower, float leftWallDist, CorrectionMode correctionMode, bool firstTick);
    void drive(float motorPower, unsigned long timeout, float leftWallDist, CorrectionMode correctionMode, float distanceToStopAt=-1);
    void turnInPlace();
    void run();

    float errDriftI;
    float errDrift;
    float left;
    float angFromWall;

    unsigned long curT;
    unsigned long deltaT;

    bool firstTick;

    float gyroTurnStartAngle;

    Hms* hms;
    Nav* nav;
    Motors* motors;
    Sensors* sensors;
};
#endif
