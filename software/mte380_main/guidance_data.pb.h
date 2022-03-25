/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_GUIDANCE_DATA_PB_H_INCLUDED
#define PB_GUIDANCE_DATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _GuidanceData_Heading { 
    GuidanceData_Heading_UP = 0, 
    GuidanceData_Heading_RIGHT = 1, 
    GuidanceData_Heading_DOWN = 2, 
    GuidanceData_Heading_LEFT = 3, 
    GuidanceData_Heading_UNKNOWN = 4 
} GuidanceData_Heading;

/* Struct definitions */
typedef struct _GuidanceData { 
    float deltaT; 
    float setpointVel; 
    float vel; 
    float errVel; 
    float errVelI; 
    float errVelD; 
    float kP_vel; 
    float kI_vel; 
    float kD_vel; 
    float velP; 
    float velI; 
    float velD; 
    float leftOutputVel; 
    float rightOutputVel; 
    float errDrift; 
    float errDriftI; 
    float errDriftD; 
    float kP_drift; 
    float kI_drift; 
    float kD_drift; 
    float driftP; 
    float driftI; 
    float driftD; 
    float leftOutputDrift; /* left motor output from drift PID */
    float rightOutputDrift; /* right motor output from drift PID */
    /* combined motor output from vel and drift PIDs (unconstrained) */
    float leftTotalPID; 
    float rightTotalPID; 
    /* constrained motor outputs */
    float leftPower; 
    float rightPower; 
    float propPower; 
    uint32_t segNum; /* current arena segment number */
    bool completedTrack; 
    GuidanceData_Heading heading; 
} GuidanceData;


/* Helper constants for enums */
#define _GuidanceData_Heading_MIN GuidanceData_Heading_UP
#define _GuidanceData_Heading_MAX GuidanceData_Heading_UNKNOWN
#define _GuidanceData_Heading_ARRAYSIZE ((GuidanceData_Heading)(GuidanceData_Heading_UNKNOWN+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define GuidanceData_init_default                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _GuidanceData_Heading_MIN}
#define GuidanceData_init_zero                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _GuidanceData_Heading_MIN}

/* Field tags (for use in manual encoding/decoding) */
#define GuidanceData_deltaT_tag                  1
#define GuidanceData_setpointVel_tag             2
#define GuidanceData_vel_tag                     3
#define GuidanceData_errVel_tag                  4
#define GuidanceData_errVelI_tag                 5
#define GuidanceData_errVelD_tag                 6
#define GuidanceData_kP_vel_tag                  7
#define GuidanceData_kI_vel_tag                  8
#define GuidanceData_kD_vel_tag                  9
#define GuidanceData_velP_tag                    10
#define GuidanceData_velI_tag                    11
#define GuidanceData_velD_tag                    12
#define GuidanceData_leftOutputVel_tag           13
#define GuidanceData_rightOutputVel_tag          14
#define GuidanceData_errDrift_tag                15
#define GuidanceData_errDriftI_tag               16
#define GuidanceData_errDriftD_tag               17
#define GuidanceData_kP_drift_tag                18
#define GuidanceData_kI_drift_tag                19
#define GuidanceData_kD_drift_tag                20
#define GuidanceData_driftP_tag                  21
#define GuidanceData_driftI_tag                  22
#define GuidanceData_driftD_tag                  23
#define GuidanceData_leftOutputDrift_tag         24
#define GuidanceData_rightOutputDrift_tag        25
#define GuidanceData_leftTotalPID_tag            26
#define GuidanceData_rightTotalPID_tag           27
#define GuidanceData_leftPower_tag               28
#define GuidanceData_rightPower_tag              29
#define GuidanceData_propPower_tag               30
#define GuidanceData_segNum_tag                  31
#define GuidanceData_completedTrack_tag          32
#define GuidanceData_heading_tag                 33

/* Struct field encoding specification for nanopb */
#define GuidanceData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    deltaT,            1) \
X(a, STATIC,   SINGULAR, FLOAT,    setpointVel,       2) \
X(a, STATIC,   SINGULAR, FLOAT,    vel,               3) \
X(a, STATIC,   SINGULAR, FLOAT,    errVel,            4) \
X(a, STATIC,   SINGULAR, FLOAT,    errVelI,           5) \
X(a, STATIC,   SINGULAR, FLOAT,    errVelD,           6) \
X(a, STATIC,   SINGULAR, FLOAT,    kP_vel,            7) \
X(a, STATIC,   SINGULAR, FLOAT,    kI_vel,            8) \
X(a, STATIC,   SINGULAR, FLOAT,    kD_vel,            9) \
X(a, STATIC,   SINGULAR, FLOAT,    velP,             10) \
X(a, STATIC,   SINGULAR, FLOAT,    velI,             11) \
X(a, STATIC,   SINGULAR, FLOAT,    velD,             12) \
X(a, STATIC,   SINGULAR, FLOAT,    leftOutputVel,    13) \
X(a, STATIC,   SINGULAR, FLOAT,    rightOutputVel,   14) \
X(a, STATIC,   SINGULAR, FLOAT,    errDrift,         15) \
X(a, STATIC,   SINGULAR, FLOAT,    errDriftI,        16) \
X(a, STATIC,   SINGULAR, FLOAT,    errDriftD,        17) \
X(a, STATIC,   SINGULAR, FLOAT,    kP_drift,         18) \
X(a, STATIC,   SINGULAR, FLOAT,    kI_drift,         19) \
X(a, STATIC,   SINGULAR, FLOAT,    kD_drift,         20) \
X(a, STATIC,   SINGULAR, FLOAT,    driftP,           21) \
X(a, STATIC,   SINGULAR, FLOAT,    driftI,           22) \
X(a, STATIC,   SINGULAR, FLOAT,    driftD,           23) \
X(a, STATIC,   SINGULAR, FLOAT,    leftOutputDrift,  24) \
X(a, STATIC,   SINGULAR, FLOAT,    rightOutputDrift,  25) \
X(a, STATIC,   SINGULAR, FLOAT,    leftTotalPID,     26) \
X(a, STATIC,   SINGULAR, FLOAT,    rightTotalPID,    27) \
X(a, STATIC,   SINGULAR, FLOAT,    leftPower,        28) \
X(a, STATIC,   SINGULAR, FLOAT,    rightPower,       29) \
X(a, STATIC,   SINGULAR, FLOAT,    propPower,        30) \
X(a, STATIC,   SINGULAR, UINT32,   segNum,           31) \
X(a, STATIC,   SINGULAR, BOOL,     completedTrack,   32) \
X(a, STATIC,   SINGULAR, UENUM,    heading,          33)
#define GuidanceData_CALLBACK NULL
#define GuidanceData_DEFAULT NULL

extern const pb_msgdesc_t GuidanceData_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define GuidanceData_fields &GuidanceData_msg

/* Maximum encoded size of messages (where known) */
#define GuidanceData_size                        178

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
