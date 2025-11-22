/* ================================================ INCLUDES =============================================== */
#include "bp.h"
#include "bp_config.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

/* ================================================= MACROS ================================================ */
#define THRESHOLD_POSITIVE_SATURATION 127 /*Note: this value is for 7 bit unsigned counter*/
#define THRESHOLD_NEGATIVE_SATURATION 0 /* See above point*/

#define USE_LONG_GHR   (0xA5A5A5A5UL)
#define USE_SHORT_GHR  (0xF0F0F0F0UL)
#define AC_COUNTER_LEN (0x9U)
#define DYNAMIC_TABLE  (6U)
/* ============================================ LOCAL VARIABLES ============================================ */
static uint32_t bpTable[MAX_NUM_OF_TABLES][MAX_NUM_OF_TABLE_ENTRIES];
static uint32_t tagTable[MAX_NUM_OF_TABLE_ENTRIES];
static uint32_t L[MAX_NUM_OF_TABLES];

static uint32_t theta_threshold = THETA_THRESHOLD;
static uint32_t threshold_counter = 0; /*TBD*/
static int32_t perceptron_sum = 0;

static uint32_t ghr             = 0;
static uint32_t aliasingCounter = 0; /* 9-bit counter */
static uint32_t useLongGhr      = 0;
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
static void BP_InitL();
static bpTableCounter_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, uint32_t ghr, uint32_t ghrLen);
static void BP_UpdateCounter(bool realOutcome, uint32_t* counter);
static uint32_t BP_IsCounterSaturated(uint32_t counterVal, uint32_t counterLen);
static uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum);
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

static uint32_t BP_IsCounterSaturated(uint32_t counterVal, uint32_t counterLen)
{
    return (counterVal == getMask(counterLen));
}

static uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum)
{
    uint32_t tag = 0;
    uint32_t tagIdx = 0;

    if ((predictedOutcome != realOutcome) && (labs(sum) <= 0))
    {
        tagIdx = BP_GetCntIdx(DYNAMIC_TABLE, currentPc, ghr, L[DYNAMIC_TABLE]);
        tag = tagTable[tagIdx];

        if ((currentPc & 1) == tag)
        {
            aliasingCounter++;
        }
        else
        {
            aliasingCounter -= 4;
        }

        tagTable[tag] = currentPc & 1;

        if (BP_IsCounterSaturated(aliasingCounter, AC_COUNTER_LEN))
        {
            return USE_LONG_GHR;
        }
        else
        {
            return USE_SHORT_GHR;
        }
    }
    return USE_SHORT_GHR;
}

static void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome)
{
    if (realOutcome!=predictedOutcome){
        threshold_counter++;
        if(threshold_counter == THRESHOLD_POSITIVE_SATURATION)
        {
            theta_threshold++;
            threshold_counter = 0;
        }
    }
    else if(abs(perceptron_sum) <= theta_threshold){
        threshold_counter--;
        if(threshold_counter == THRESHOLD_NEGATIVE_SATURATION)
        {
            theta_threshold--;
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

uint32_t BP_GetLIndex(uint32_t useLongGhr, uint32_t tableIndex)
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

void BP_Update(bool realOutcome, bool predictedOutcome, uint32_t currentPc, uint32_t nextPc, int32_t sum)
{
    uint32_t cntIdx = 0;
    uint32_t lIndex = 0;

    /* 3.1.1 Update the predictor counters */
    if (predictedOutcome != realOutcome || labs(sum) < THETA_THRESHOLD)
    {
        for (int tableIdx = 0; tableIdx < gNumOfTables; tableIdx++)
        {
            lIndex = BP_GetLIndex(useLongGhr, tableIdx);
            cntIdx = BP_GetCntIdx(tableIdx, currentPc, ghr, L[lIndex]);

            BP_UpdateCounter(realOutcome, &bpTable[tableIdx][cntIdx]);
        }
    }

    /* 3.2 Dynamic history length fitting */
    useLongGhr = BP_GetAliasingRatio(realOutcome, predictedOutcome, currentPc, sum);

    /* 3.3 Adaptive threshold fitting */
    BP_UpdateThreshold(realOutcome,predictedOutcome);

    /* update ghr */
    ghr = (ghr << 1) | realOutcome;
}
