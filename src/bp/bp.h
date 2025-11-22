#ifndef BP
#define BP

/* ================================================ INCLUDES =============================================== */
#include <stdint.h>
#include "bp_types.h"
#include "stdbool.h"

/* ================================================= MACROS ================================================ */
/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* =============================================== MODULE API ============================================== */
void BP_Init(void);
bool BP_GetPrediction(uint32_t pc, int32_t* sum);
void BP_Update(bool realOutcome, bool predictedOutcome, uint32_t currentPc, uint32_t nextPc, int32_t sum);

#endif /* BP */
