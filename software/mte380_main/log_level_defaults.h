#ifndef LOG_LEVEL_DEFAULTS
#define LOG_LEVEL_DEFAULTS
#include "hms_and_cmd_data.pb.h"

#define MAIN_LOG_LEVEL_DEFAULT HmsData_LogLevel_NORMAL
#define SENSORS_LOG_LEVEL_DEFAULT HmsData_LogLevel_NORMAL
#define NAV_LOG_LEVEL_DEFAULT HmsData_LogLevel_OVERKILL
#define GUIDANCE_LOG_LEVEL_DEFAULT HmsData_LogLevel_NORMAL

#endif
