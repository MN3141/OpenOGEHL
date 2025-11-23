/* ================================================ INCLUDES =============================================== */
#include "bp.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "bp_types.h"
#include "utils.h"
#include "bp_config.h"
#include "bp_defines.h"

/* ================================================= MACROS ================================================ */
#define USE_LONG_GHR            (0xA5A5A5A5UL)
#define USE_SHORT_GHR           (0xF0F0F0F0UL)
#define AC_COUNTER_LEN          (0x9U)
#define DYNAMIC_TABLE           (6U)
#define THRESHOLD_COUNTER_LEN   (7U)

/* ============================================ LOCAL VARIABLES ============================================ */
BP_STATIC bpCounter_t bpTable[MAX_NUM_OF_TABLES][MAX_NUM_OF_TABLE_ENTRIES];
BP_STATIC bpTag_t tagTable[MAX_NUM_OF_TABLE_ENTRIES];
BP_STATIC size_t L[MAX_NUM_OF_TABLES];

BP_STATIC uint32_t theta_threshold      = THETA_THRESHOLD;
BP_STATIC uint32_t threshold_counter    = 0; /*TBD*/
BP_STATIC int32_t perceptron_sum        = 0;
BP_STATIC bpGhr_t ghr                   = 0;
BP_STATIC uint32_t aliasingCounter      = 0; /* 9-bit counter */
BP_STATIC uint32_t useLongGhr           = 0;

/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
BP_STATIC void BP_InitL();
BP_STATIC bpCounter_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, bpGhr_t ghr, size_t ghrLen);
BP_STATIC uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum);
BP_STATIC void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome);
BP_STATIC uint32_t BP_GetLIndex(uint32_t useLongGhr, uint32_t tableIndex);

/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
BP_STATIC void BP_InitL(void)
{
    for (int i = 0; i < MAX_NUM_OF_TABLES; i++)
    {
        L[i] = (uint32_t)((pow(L_ALPHA, i - 1)) * L1 + 0.5);
    }
}

BP_STATIC uint32_t BP_GetLIndex(uint32_t useLongGhr, uint32_t tableIndex)
{
    if (useLongGhr == USE_LONG_GHR)
    {
        switch (tableIndex)
        {
            case 2:
                return gNumOfTables + 1;
            case 4:
                return gNumOfTables + 2;
            case 6:
                return gNumOfTables + 3;
        }
    }

    return tableIndex;
}

BP_STATIC bpCounter_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, bpGhr_t lGhr, size_t ghrLen)
{
    if (tableIdx == 0)
    {
        return pc;
    }

    return pc ^ (lGhr & getMask(ghrLen));
}

BP_STATIC uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum)
{
    uint32_t tag = 0;
    uint32_t tagIdx = 0;

    if ((predictedOutcome != realOutcome) /* Missprediction */ && (labs(sum) <= 0))
    {
        tagIdx = BP_GetCntIdx(DYNAMIC_TABLE, currentPc, ghr, L[DYNAMIC_TABLE]);
        tag = tagTable[tagIdx];

        if ((currentPc & 1) == tag)
        {
            addValToCounter(&aliasingCounter, sizeof(aliasingCounter), AC_COUNTER_LEN, 1 /* value */);
        }
        else
        {
            addValToCounter(&aliasingCounter, sizeof(aliasingCounter), AC_COUNTER_LEN, -4 /* value */);
        }

        tagTable[tag] = currentPc & 1;

        if (CNT_SATURATED_POSITIVE == getCntSaturation(aliasingCounter, AC_COUNTER_LEN))
        {
            return USE_LONG_GHR;
        }

        if (CNT_SATURATED_NEGATIVE == getCntSaturation(aliasingCounter, AC_COUNTER_LEN))
        {
            return USE_SHORT_GHR;
        }
    }

    return USE_SHORT_GHR;
}

BP_STATIC void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome)
{
    if (realOutcome!=predictedOutcome){
        addValToCounter(&threshold_counter, sizeof(threshold_counter), THRESHOLD_COUNTER_LEN, 1 /* value */);

        if(CNT_SATURATED_POSITIVE == getCntSaturation(threshold_counter, THRESHOLD_COUNTER_LEN))
        {
            addValToCounter(&theta_threshold, sizeof(theta_threshold), 8U, 1 /* value */); /* TODO: Theta is not a counter so it doesnt have a well defined length */
            threshold_counter = 0;
        }
    }
    else if(abs(perceptron_sum) <= theta_threshold){
        addValToCounter(&threshold_counter, sizeof(threshold_counter), THRESHOLD_COUNTER_LEN, -1 /* value */);

        if(CNT_SATURATED_NEGATIVE == getCntSaturation(threshold_counter, THRESHOLD_COUNTER_LEN))
        {
            addValToCounter(&theta_threshold, sizeof(theta_threshold), 8U, -1 /* value */);/* TODO: Theta is not a counter so it doesnt have a well defined length */
            threshold_counter = 0;
        }
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
    perceptron_sum = *sum;

    return (*sum >= 0);
}

void BP_Update(bool realOutcome, bool predictedOutcome, uint32_t currentPc, uint32_t nextPc, int32_t sum)
{
    uint32_t cntIdx = 0;
    uint32_t lIndex = 0;

    /* 3.1.1 Update the predictor counters */
    if ((predictedOutcome != realOutcome) /* Missprediction */ || (labs(sum) <= THETA_THRESHOLD) /* Outliar or idk */)
    {
        for (int tableIdx = 0; tableIdx < gNumOfTables; tableIdx++)
        {
            lIndex = BP_GetLIndex(useLongGhr, tableIdx);
            cntIdx = BP_GetCntIdx(tableIdx, currentPc, ghr, L[lIndex]);

            if (realOutcome)
            {
                addValToCounter(&bpTable[tableIdx][cntIdx], sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 1 /* value */);
            }
            else
            {
                addValToCounter(&bpTable[tableIdx][cntIdx], sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -1 /* value */);
            }
        }
    }

    /* 3.2 Dynamic history length fitting */
    useLongGhr = BP_GetAliasingRatio(realOutcome, predictedOutcome, currentPc, sum);

    /* 3.3 Adaptive threshold fitting */
    BP_UpdateThreshold(realOutcome,predictedOutcome);

    /* update ghr */
    ghr = (ghr << 1) | realOutcome;
}
