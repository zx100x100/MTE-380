#include "hms.h"
#include "log_level_defaults.h"

#define DEFAULT_N_BATTERY_CELLS 2

Hms::Hms()
{
  navData = NavData_init_zero;
  HmsData data = HmsData_init_zero;
  /* HmsData data_ = HmsData_init_zero; */
  /* data = data_; */
  data.nCells = DEFAULT_N_BATTERY_CELLS;

  // data.mainLogLevel = MAIN_LOG_LEVEL_DEFAULT;
  // data.sensorsLogLevel = SENSORS_LOG_LEVEL_DEFAULT;
  // data.guidanceLogLevel = GUIDANCE_LOG_LEVEL_DEFAULT;
  // data.navLogLevel = NAV_LOG_LEVEL_DEFAULT;
}

void Hms::logError(int err, char str[80]){
  Serial.print("Error: "); Serial.print(err); Serial.println(str);
}
