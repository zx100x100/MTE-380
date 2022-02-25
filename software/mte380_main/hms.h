#ifndef HMS_H
#define HMS_H

#include "hms_data.pb.h"
#include "nav_data.pb.h"
#include "Arduino.h"

class Hms{
  public:
    Hms();
    void logError(int err, char str[80]);
    HmsData& data();
    HmsData hmsData;

  private:
    NavData navData;
};

#endif
