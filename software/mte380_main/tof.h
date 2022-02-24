#ifndef TOF_H
#define TOF_H

#include "tof_data.pb.h"

class Tof{
  public:
    Tof();
    void poll();
    TofData& getData();
    /* void displayDetails(); */

  private:
    TofData tofData;

};

#endif
