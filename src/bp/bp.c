/* ================================================ INCLUDES =============================================== */
#include "bp.h"
#include "bp_config.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
static uint32_t ghr = 0;
static uint32_t bpTable[MAX_NUM_OF_TABLES][MAX_NUM_OF_TABLE_ENTRIES];
static uint32_t L[MAX_NUM_OF_TABLES];
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
static void BP_InitL();
static bpTableCounter_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, uint32_t ghr, uint32_t ghrLen);
static void BP_UpdateCounter(bool realOutcome, uint32_t* counter);
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
static void BP_InitL(void)
{
    for (int i = 0; i < MAX_NUM_OF_TABLES; i++)
    {
        L[i] = ((pow(L_ALPHA, i) - 1) * L1 + 0.5);
    }
}

static bpTableCounter_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, uint32_t lGhr, uint32_t ghrLen)
{
    if (tableIdx == 0)
    {
        return pc;
    }

    return pc ^ (lGhr & getMask(ghrLen));
}

static void BP_UpdateCounter(bool realOutcome, uint32_t* counter)
{
    if (realOutcome)
    {
         /* TAKEN */
        *counter += 1;
    }
    else
    {
         /* NOT TAKEN */
        *counter -= 1;
    }
}

/* ================================================ MODULE API ============================================= */
void BP_Init(void)
{
    BP_InitL();
}

bool BP_GetPrediction(uint32_t pc, int32_t* sum)
{
    uint32_t cntIdx = 0;
    int64_t cnt = 0;

    for (int tableIdx = 0; tableIdx < gNumOfTables; tableIdx++)
    {
        cntIdx = BP_GetCntIdx(tableIdx, pc, ghr, L[tableIdx]);

        cnt = bpTable[tableIdx][cntIdx];

        *sum += cnt;
    }
    *sum += (gNumOfTables/2);

    return (*sum >= 0);
}

void BP_Update(bool realOutcome, bool predictedOutcome, uint32_t currentPc, uint32_t nextPc, int32_t sum)
{
    uint32_t cntIdx = 0;

    if (predictedOutcome != realOutcome || labs(sum) < THETA_THRESHOLD)
    {
        for (int tableIdx = 0; tableIdx < gNumOfTables; tableIdx++)
        {
            cntIdx = BP_GetCntIdx(tableIdx, currentPc, ghr, L[tableIdx]);

            BP_UpdateCounter(realOutcome, &bpTable[tableIdx][cntIdx]);
        }
    }

    /* TODO: Find out if we need to implement:
     * 3.2 Dynamic history length fitting
     * 3.3 Adaptive threshold fitting
     * */
}
