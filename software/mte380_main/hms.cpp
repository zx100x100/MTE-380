#include "hms.h"

#define DEFAULT_N_BATTERY_CELLS 2

Hms::Hms()
{
  HmsData data_ = HmsData_init_zero;
  data = data_;
  data.nCells = DEFAULT_N_BATTERY_CELLS;
}

void Hms::logError(int err, String str){
  Serial.print("Error: "); Serial.print(err); Serial.println(str);
}
