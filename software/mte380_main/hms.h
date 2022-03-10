#ifndef HMS_H
#define HMS_H

#include "hms_and_cmd_data.pb.h"
#include "nav_data.pb.h"
#include "Arduino.h"

class Hms{
  public:
    Hms();
    void init();
    void logError(int err, char str[80]);
    HmsData data;

  private:
    NavData navData;
};

#endif
