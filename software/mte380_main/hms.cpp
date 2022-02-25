#include "hms.h"

#define DEFAULT_N_BATTERY_CELLS 2

Hms::Hms()
{
  navData = NavData_init_zero;
  HmsData hmsData = HmsData_init_zero;
  /* HmsData data_ = HmsData_init_zero; */
  /* hmsData = data_; */
  hmsData.nCells = DEFAULT_N_BATTERY_CELLS;
}

void Hms::logError(int err, char str[80]){
  Serial.print("Error: "); Serial.print(err); Serial.println(str);
}

HmsData& Hms::data(){
  return hmsData;
}
