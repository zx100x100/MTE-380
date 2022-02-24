#ifndef TOF_H
#define TOF_H

#include "hms.h"
#include "tof_data.pb.h"

class Tof{
  public:
    Tof();
    Tof(Hms* hms);
    void poll();
    TofData& getData();
    /* void displayDetails(); */

  private:
    TofData tofData;
    Hms* hms;

};

#endif
