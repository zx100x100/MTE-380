#ifndef HMS_H
#define HMS_H

#include "hms_data.pb.h"
#include "Arduino.h"

class Hms{
  public:
    Hms();
    void logError(int err, String str);
    HmsData data;
};

#endif
