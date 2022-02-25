#include "tof.h"

/* Tof::Tof(){ */
/* } */
Tof::Tof(){}

Tof::Tof(Hms* hms):
  hms(hms)
{
  tofData = TofData_init_zero;
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
  tofData.dist = 5;
  /* hms.data().errorInfo"blah"; */
  /* hms->data.batteryVoltage = 2.0; */
  return;
}
