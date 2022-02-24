#include "tof.h"

Tof::Tof(){
  tofData = TofData_init_zero;
}

TofData& Tof::getData(){
  return tofData;
}

void Tof::poll(){
  return;
}
