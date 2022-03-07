#include "hms.h"

#define DEFAULT_N_BATTERY_CELLS 2

Hms::Hms()
{
  navData = NavData_init_zero;
  HmsData data = HmsData_init_zero;
  /* HmsData data_ = HmsData_init_zero; */
  /* data = data_; */
  data.nCells = DEFAULT_N_BATTERY_CELLS;
  data.logLevel = HmsData_LogLevel_NORMAL;
}

void Hms::logError(int err, char str[80]){
  Serial.print("Error: "); Serial.print(err); Serial.println(str);
}
