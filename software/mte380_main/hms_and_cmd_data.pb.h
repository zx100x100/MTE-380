/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_HMS_AND_CMD_DATA_PB_H_INCLUDED
#define PB_HMS_AND_CMD_DATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _HmsData_Error { 
    HmsData_Error_LOW_BATTERY = 0, 
    HmsData_Error_WTF_AHMAD = 1 
} HmsData_Error;

typedef enum _HmsData_LogLevel { 
    HmsData_LogLevel_NORMAL = 0, 
    HmsData_LogLevel_DEBUG = 1, 
    HmsData_LogLevel_OVERKILL = 2 
} HmsData_LogLevel;

typedef enum _CmdData_RunState { 
    CmdData_RunState_E_STOP = 0, 
    CmdData_RunState_AUTO = 1, 
    CmdData_RunState_TELEOP = 2, 
    CmdData_RunState_SIM = 3 
} CmdData_RunState;

/* Struct definitions */
typedef struct _CmdData { 
    int32_t placeholder; 
    float leftPower; 
    float rightPower; 
    float propPower; 
    CmdData_RunState runState; 
    HmsData_LogLevel mainLogLevel; 
    HmsData_LogLevel sensorsLogLevel; 
    HmsData_LogLevel navLogLevel; 
    HmsData_LogLevel guidanceLogLevel; 
    uint32_t nTraps; 
    float trapX[8]; 
    float trapY[8]; 
    float simPosX; 
    float simPosY; 
    float simVelX; 
    float simVelY; 
    float simAccX; 
    float simAccY; 
    float simAngXy; 
    float simAngVelXy; 
    float angAccXy; /* angular acceleration */
} CmdData;

typedef struct _HmsData { 
    float batteryVoltage; 
    uint32_t nCells; 
    HmsData_LogLevel mainLogLevel; 
    HmsData_LogLevel sensorsLogLevel; 
    HmsData_LogLevel navLogLevel; 
    HmsData_LogLevel guidanceLogLevel; 
    uint32_t networkTickRate; 
    uint32_t mainTickRate; 
    uint32_t sensorsTickRate; 
    uint32_t combinedTickRate; 
    uint32_t longestCombinedTick; 
    char errorInfo[80]; /* MUST ALWAYS BE THE LAST ENTRY!! (due to some janky dashboard code) */
} HmsData;


/* Helper constants for enums */
#define _HmsData_Error_MIN HmsData_Error_LOW_BATTERY
#define _HmsData_Error_MAX HmsData_Error_WTF_AHMAD
#define _HmsData_Error_ARRAYSIZE ((HmsData_Error)(HmsData_Error_WTF_AHMAD+1))

#define _HmsData_LogLevel_MIN HmsData_LogLevel_NORMAL
#define _HmsData_LogLevel_MAX HmsData_LogLevel_OVERKILL
#define _HmsData_LogLevel_ARRAYSIZE ((HmsData_LogLevel)(HmsData_LogLevel_OVERKILL+1))

#define _CmdData_RunState_MIN CmdData_RunState_E_STOP
#define _CmdData_RunState_MAX CmdData_RunState_SIM
#define _CmdData_RunState_ARRAYSIZE ((CmdData_RunState)(CmdData_RunState_SIM+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define HmsData_init_default                     {0, 0, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, 0, 0, 0, 0, 0, ""}
#define CmdData_init_default                     {0, 0, 0, 0, _CmdData_RunState_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, 0, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define HmsData_init_zero                        {0, 0, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, 0, 0, 0, 0, 0, ""}
#define CmdData_init_zero                        {0, 0, 0, 0, _CmdData_RunState_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, _HmsData_LogLevel_MIN, 0, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define CmdData_placeholder_tag                  1
#define CmdData_leftPower_tag                    2
#define CmdData_rightPower_tag                   3
#define CmdData_propPower_tag                    4
#define CmdData_runState_tag                     5
#define CmdData_mainLogLevel_tag                 6
#define CmdData_sensorsLogLevel_tag              7
#define CmdData_navLogLevel_tag                  8
#define CmdData_guidanceLogLevel_tag             9
#define CmdData_nTraps_tag                       10
#define CmdData_trapX_tag                        11
#define CmdData_trapY_tag                        12
#define CmdData_simPosX_tag                      13
#define CmdData_simPosY_tag                      14
#define CmdData_simVelX_tag                      15
#define CmdData_simVelY_tag                      16
#define CmdData_simAccX_tag                      17
#define CmdData_simAccY_tag                      18
#define CmdData_simAngXy_tag                     19
#define CmdData_simAngVelXy_tag                  20
#define CmdData_angAccXy_tag                     21
#define HmsData_batteryVoltage_tag               1
#define HmsData_nCells_tag                       2
#define HmsData_mainLogLevel_tag                 3
#define HmsData_sensorsLogLevel_tag              4
#define HmsData_navLogLevel_tag                  5
#define HmsData_guidanceLogLevel_tag             6
#define HmsData_networkTickRate_tag              7
#define HmsData_mainTickRate_tag                 8
#define HmsData_sensorsTickRate_tag              9
#define HmsData_combinedTickRate_tag             10
#define HmsData_longestCombinedTick_tag          11
#define HmsData_errorInfo_tag                    12

/* Struct field encoding specification for nanopb */
#define HmsData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    batteryVoltage,    1) \
X(a, STATIC,   SINGULAR, UINT32,   nCells,            2) \
X(a, STATIC,   SINGULAR, UENUM,    mainLogLevel,      3) \
X(a, STATIC,   SINGULAR, UENUM,    sensorsLogLevel,   4) \
X(a, STATIC,   SINGULAR, UENUM,    navLogLevel,       5) \
X(a, STATIC,   SINGULAR, UENUM,    guidanceLogLevel,   6) \
X(a, STATIC,   SINGULAR, UINT32,   networkTickRate,   7) \
X(a, STATIC,   SINGULAR, UINT32,   mainTickRate,      8) \
X(a, STATIC,   SINGULAR, UINT32,   sensorsTickRate,   9) \
X(a, STATIC,   SINGULAR, UINT32,   combinedTickRate,  10) \
X(a, STATIC,   SINGULAR, UINT32,   longestCombinedTick,  11) \
X(a, STATIC,   SINGULAR, STRING,   errorInfo,        12)
#define HmsData_CALLBACK NULL
#define HmsData_DEFAULT NULL

#define CmdData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    placeholder,       1) \
X(a, STATIC,   SINGULAR, FLOAT,    leftPower,         2) \
X(a, STATIC,   SINGULAR, FLOAT,    rightPower,        3) \
X(a, STATIC,   SINGULAR, FLOAT,    propPower,         4) \
X(a, STATIC,   SINGULAR, UENUM,    runState,          5) \
X(a, STATIC,   SINGULAR, UENUM,    mainLogLevel,      6) \
X(a, STATIC,   SINGULAR, UENUM,    sensorsLogLevel,   7) \
X(a, STATIC,   SINGULAR, UENUM,    navLogLevel,       8) \
X(a, STATIC,   SINGULAR, UENUM,    guidanceLogLevel,   9) \
X(a, STATIC,   SINGULAR, UINT32,   nTraps,           10) \
X(a, STATIC,   FIXARRAY, FLOAT,    trapX,            11) \
X(a, STATIC,   FIXARRAY, FLOAT,    trapY,            12) \
X(a, STATIC,   SINGULAR, FLOAT,    simPosX,          13) \
X(a, STATIC,   SINGULAR, FLOAT,    simPosY,          14) \
X(a, STATIC,   SINGULAR, FLOAT,    simVelX,          15) \
X(a, STATIC,   SINGULAR, FLOAT,    simVelY,          16) \
X(a, STATIC,   SINGULAR, FLOAT,    simAccX,          17) \
X(a, STATIC,   SINGULAR, FLOAT,    simAccY,          18) \
X(a, STATIC,   SINGULAR, FLOAT,    simAngXy,         19) \
X(a, STATIC,   SINGULAR, FLOAT,    simAngVelXy,      20) \
X(a, STATIC,   SINGULAR, FLOAT,    angAccXy,         21)
#define CmdData_CALLBACK NULL
#define CmdData_DEFAULT NULL

extern const pb_msgdesc_t HmsData_msg;
extern const pb_msgdesc_t CmdData_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define HmsData_fields &HmsData_msg
#define CmdData_fields &CmdData_msg

/* Maximum encoded size of messages (where known) */
#define CmdData_size                             173
#define HmsData_size                             130

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
