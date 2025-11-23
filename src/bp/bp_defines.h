#ifndef BP_DEFINES
#define BP_DEFINES

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
#if defined(UNIT_TESTS)
#define BP_STATIC
#else
#define BP_STATIC static
#endif

/* ************************************************* FEATURE *********************************************** */
#define USE_DYNAMIC_HISTORY_LENGTH (TRUE)

/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* =============================================== MODULE API ============================================== */

#endif /* BP_DEFINES */
