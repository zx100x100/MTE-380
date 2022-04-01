#ifndef SORRY_H
#define SORRY_H
#include "motors.h"
#include "sensors.h"
#include "hms.h"

#define NO_PITCH_ANGLE_THRESHOLD -1000

enum CorrectionMode{ UNGUIDED, GUIDED, PARALLEL, GUIDED_GYRO };

class Sorry{
  public:
    Sorry();
    Sorry(Motors* motors, Sensors* sensors, Hms* hms);

    bool isValid(int tofNum);
    float getTofFt(int tofNum);
    bool updateWallAngleAndDistance(float gyroAngle, float desiredDistToLeftWall, bool firstTick);
    void driveTick(float motorPower, float leftWallDist, CorrectionMode correctionMode, bool firstTick);
    float getDirectionCorrectedGyroAngle();
    float getDriftCorrectedGyroAngle(unsigned long microsSinceLastZeroed);
    float bringGyroMeasurementIntoPositiveDegreesUsingNumClockwiseWraparounds(float rawAngle);
    void calibrateGyroDrift();
    void drive(int num, int segment, float motorPower, unsigned long timeout, float desiredDistToLeftWall, CorrectionMode correctionMode, float distanceToStopAt=-1, float pitchToStopAt = NO_PITCH_ANGLE_THRESHOLD, bool isMinimumPitchThreshold=false);
    void turnInPlace(int num);
    void run();

    float angleErrorI;
    float curDistToLeftWall;
    float angFromWall;
    float gyroAngleAtLastValidWallAngle;
    float lastValidWallAngle;

    unsigned long curT;
    unsigned long deltaT;

    bool firstTick;

    Hms* hms;
    Motors* motors;
    Sensors* sensors;

    unsigned long startCurDriveSegmentT;
    bool prevGyroMeasurementWasLargeAndMightWrapAroundToNegative;
    int numGyroClockwiseWraparounds = 0;
    float gyroDriftPerMicro;
    float startCurDriveSegmentAngle;
    float angleError;
    float lastLeftTotalPID;
    float lastRightTotalPID;
};
#endif
