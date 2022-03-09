/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_TOF_DATA_PB_H_INCLUDED
#define PB_TOF_DATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _TofData { 
    uint32_t dist; 
} TofData;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define TofData_init_default                     {0}
#define TofData_init_zero                        {0}

/* Field tags (for use in manual encoding/decoding) */
#define TofData_dist_tag                         1

/* Struct field encoding specification for nanopb */
#define TofData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   dist,              1)
#define TofData_CALLBACK NULL
#define TofData_DEFAULT NULL

extern const pb_msgdesc_t TofData_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define TofData_fields &TofData_msg

/* Maximum encoded size of messages (where known) */
#define TofData_size                             6

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
