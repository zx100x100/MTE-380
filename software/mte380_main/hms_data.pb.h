/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_HMS_DATA_PB_H_INCLUDED
#define PB_HMS_DATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _HmsData_Error { 
    HmsData_Error_LOW_BATTERY = 0, 
    HmsData_Error_OFF_TRACK = 1 
} HmsData_Error;

/* Struct definitions */
typedef struct _HmsData { 
    float batteryVoltage; 
    uint32_t nCells; 
    char errorInfo[80]; /* Error error = 4; */
} HmsData;


/* Helper constants for enums */
#define _HmsData_Error_MIN HmsData_Error_LOW_BATTERY
#define _HmsData_Error_MAX HmsData_Error_OFF_TRACK
#define _HmsData_Error_ARRAYSIZE ((HmsData_Error)(HmsData_Error_OFF_TRACK+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define HmsData_init_default                     {0, 0, ""}
#define HmsData_init_zero                        {0, 0, ""}

/* Field tags (for use in manual encoding/decoding) */
#define HmsData_batteryVoltage_tag               1
#define HmsData_nCells_tag                       2
#define HmsData_errorInfo_tag                    3

/* Struct field encoding specification for nanopb */
#define HmsData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    batteryVoltage,    1) \
X(a, STATIC,   SINGULAR, UINT32,   nCells,            2) \
X(a, STATIC,   SINGULAR, STRING,   errorInfo,         3)
#define HmsData_CALLBACK NULL
#define HmsData_DEFAULT NULL

extern const pb_msgdesc_t HmsData_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define HmsData_fields &HmsData_msg

/* Maximum encoded size of messages (where known) */
#define HmsData_size                             92

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
