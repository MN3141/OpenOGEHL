#ifndef BP
#define BP

/* ================================================ INCLUDES =============================================== */
#include <stdint.h>
#include "bp_types.h"
#include "stdbool.h"
#include "bp_defines.h"

/* ================================================= MACROS ================================================ */
#define USE_LONG_GHR            (0xA5A5A5A5UL)
#define USE_SHORT_GHR           (0xF0F0F0F0UL)
/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* 4 - 12 tables */
extern uint32_t gNumOfTables;
/* 32Kb - 1Mb size */
extern uint32_t gTableSize;
/* 3 - 5 bit counters */
extern uint32_t gCounterLen;

/* =============================================== MODULE API ============================================== */
void BP_Init(void);
bool BP_GetPrediction(uint32_t pc, int32_t* sum);
void BP_Update(bool realOutcome, bool predictedOutcome, uint32_t currentPc, uint32_t nextPc, int32_t sum);

#endif /* BP */
