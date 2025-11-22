#ifndef BP_CONFIG
#define BP_CONFIG

/* ================================================ INCLUDES =============================================== */
#include <stdint.h>
/* ================================================= MACROS ================================================ */
#define DEFAULT_NUM_OF_TABLES    (7U)
#define MAX_NUM_OF_TABLES        (11U)
#define MAX_NUM_OF_TABLE_ENTRIES (2048U)
#define L_ALPHA                  (2U)
#define L1                       (2U)
#define THETA_THRESHOLD          (DEFAULT_NUM_OF_TABLES)
#define DEFAULT_COUNTER_LEN      (4U)
/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
uint32_t gNumOfTables = DEFAULT_NUM_OF_TABLES;
/* =============================================== MODULE API ============================================== */

#endif /* BP_CONFIG */
