/* ================================================ INCLUDES =============================================== */
#include "bp.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bp_types.h"
#include "utils.h"
#include "bp_defines.h"

/* ================================================= MACROS ================================================ */
#define AC_COUNTER_LEN          (0x9U)
#define DYNAMIC_TABLE           (6U)
#define THRESHOLD_COUNTER_LEN   (7U)

/* ============================================ LOCAL VARIABLES ============================================ */
BP_STATIC bpCounter_t bpTable[MAX_NUM_OF_TABLES][MAX_NUM_OF_TABLE_ENTRIES];
BP_STATIC bpTag_t tagTable[MAX_NUM_OF_TABLE_ENTRIES];
BP_STATIC size_t L[MAX_NUM_OF_TABLES];

BP_STATIC uint32_t theta_threshold      = 0;
BP_STATIC uint32_t threshold_counter    = 0;
BP_STATIC bpGhr_t ghr                   = 0;
BP_STATIC uint32_t aliasingCounter      = 0; /* 9-bit counter */
BP_STATIC uint32_t useLongGhr           = 0;

/* ============================================ GLOBAL VARIABLES =========================================== */
/* 4 - 12 tables */
uint32_t gNumOfTables = DEFAULT_NUM_OF_TABLES;

/* 32Kb - 1Mb size */
uint32_t gTableSize   = DEFAULT_NUM_OF_TABLE_ENTRIES;

/* 3 - 5 bit counters */
uint32_t gCounterLen  = DEFAULT_COUNTER_LEN;

/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
BP_STATIC void BP_InitL();
BP_STATIC uint32_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, bpGhr_t ghr, size_t ghrLen);
BP_STATIC uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum);
BP_STATIC void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome, int32_t sum);
BP_STATIC uint32_t BP_GetLIndex(uint32_t useLongGhr, uint32_t tableIndex);

/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
BP_STATIC void BP_InitL(void)
{
    for (int i = 0; i < gNumOfTables; i++)
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

BP_STATIC uint32_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, bpGhr_t lGhr, size_t ghrLen)
{
    uint32_t index;

    if (tableIdx == 0)
    {
        index = pc;
    }
    else
    {
        index = pc ^ (lGhr & getMask(ghrLen));
    }

    /* Ensure index is within bounds of the table */
    return index % gTableSize;
}

BP_STATIC uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum)
{
    uint32_t tag    = 0;
    uint32_t tagIdx = 0;

    if ((predictedOutcome != realOutcome) /* Missprediction */ && (labs(sum) <= theta_threshold))
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

BP_STATIC void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome, int32_t sum)
{
    if (realOutcome != predictedOutcome)
    {
        addValToCounter(&threshold_counter, sizeof(threshold_counter), THRESHOLD_COUNTER_LEN, 1 /* value */);

        if(CNT_SATURATED_POSITIVE == getCntSaturation(threshold_counter, THRESHOLD_COUNTER_LEN))
        {
            addValToCounter(&theta_threshold, sizeof(theta_threshold), 8U, 1 /* value */); /* TODO: Theta is not a counter so it doesnt have a well defined length */
            threshold_counter = 0;
        }
    }
    else if(abs(sum) <= theta_threshold)
    {
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
    memset(bpTable, 0, sizeof(bpTable));
    memset(tagTable, 0, sizeof(tagTable));
    memset(L, 0, sizeof(L));

    theta_threshold   = gNumOfTables;
    threshold_counter = 0; /*TBD*/
    aliasingCounter   = 0; /* 9-bit counter */
    useLongGhr        = 0;
    ghr               = 0;

    BP_InitL();
}

bool BP_GetPrediction(uint32_t pc, int32_t* sum)
{
    uint32_t cntIdx = 0;
    int64_t cnt = 0;

    *sum = 0;

    for (int tableIdx = 0; tableIdx < gNumOfTables; tableIdx++)
    {
        cntIdx = BP_GetCntIdx(tableIdx, pc, ghr, L[tableIdx]);

        cnt = bpTable[tableIdx][cntIdx];

        *sum += cnt;
    }

    return (*sum >= 0);  /* Use > 0 to predict NOT TAKEN on cold start (when sum == 0) */
}

void BP_Update(bool realOutcome, bool predictedOutcome, uint32_t currentPc, uint32_t nextPc, int32_t sum)
{
    uint32_t cntIdx = 0;
    uint32_t lIndex = 0;

    /* 3.1.1 Update the predictor counters */
    if ((predictedOutcome != realOutcome) /* Missprediction */ || (labs(sum) <= theta_threshold) /* Outliar or idk */)
    {
        for (int tableIdx = 0; tableIdx < gNumOfTables; tableIdx++)
        {
            lIndex = BP_GetLIndex(useLongGhr, tableIdx);
            cntIdx = BP_GetCntIdx(tableIdx, currentPc, ghr, L[lIndex]);

            if (realOutcome)
            {
                addValToCounter(&bpTable[tableIdx][cntIdx], sizeof(bpCounter_t), gCounterLen, 1 /* value */);
            }
            else
            {
                addValToCounter(&bpTable[tableIdx][cntIdx], sizeof(bpCounter_t), gCounterLen, -1 /* value */);
            }
        }
    }

    /* 3.2 Dynamic history length fitting */
    useLongGhr = BP_GetAliasingRatio(realOutcome, predictedOutcome, currentPc, sum);

    /* 3.3 Adaptive threshold fitting */
    BP_UpdateThreshold(realOutcome, predictedOutcome, sum);

    /* update ghr */
    ghr = (ghr << 1) | realOutcome;
}
