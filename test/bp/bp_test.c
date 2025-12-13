/* ================================================ INCLUDES =============================================== */
#include "bp_test.h"
#include "bp.h"
#include "bp_defines.h"
#include "bp_types.h"
#include "unity.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>
/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
extern bpCounter_t bpTable[MAX_NUM_OF_TABLES][MAX_NUM_OF_TABLE_ENTRIES];
extern bpTag_t tagTable[MAX_NUM_OF_TABLE_ENTRIES];
extern size_t L[MAX_NUM_OF_TABLES];

extern uint32_t theta_threshold;
extern uint32_t threshold_counter; /*TBD*/
extern int32_t perceptron_sum;

extern bpGhr_t ghr;
extern uint32_t aliasingCounter; /* 9-bit counter */
extern uint32_t useLongGhr;

/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
extern void BP_InitL();
extern bpCounter_t BP_GetCntIdx(uint32_t tableIdx, uint32_t pc, bpGhr_t ghr, size_t ghrLen);
extern uint32_t BP_GetAliasingRatio(bool realOutcome, bool predictedOutcome, uint32_t currentPc, int32_t sum);
extern void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome, int32_t sum);
extern uint32_t BP_GetLIndex(uint32_t useLongGhr, uint32_t tableIndex);

/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
/**
 * @brief Ensures that the L array gets expected values
 * */
void test_BP_InitL(void)
{
    BP_InitL();

    for (int i = 0; i < MAX_NUM_OF_TABLES; i++)
    {
        LOG("%d ", L[i]);
    }

    LOG("\n");

    TEST_ASSERT_EQUAL_UINT32(1,    L[0]);
    TEST_ASSERT_EQUAL_UINT32(2,    L[1]);
    TEST_ASSERT_EQUAL_UINT32(4,    L[2]);
    TEST_ASSERT_EQUAL_UINT32(8,    L[3]);
    TEST_ASSERT_EQUAL_UINT32(16,   L[4]);
    TEST_ASSERT_EQUAL_UINT32(32,   L[5]);
    TEST_ASSERT_EQUAL_UINT32(64,   L[6]);
    TEST_ASSERT_EQUAL_UINT32(128,  L[7]);
    TEST_ASSERT_EQUAL_UINT32(256,  L[8]);
    TEST_ASSERT_EQUAL_UINT32(512,  L[9]);
    TEST_ASSERT_EQUAL_UINT32(1024, L[10]);
}

/**
 * @brief Test the BP_GetCntIdx at cold start
 * */
void test_BP_GetCntIdx_GhrIsEmpty(void)
{
    // BS 2 151
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(0, 2, ghr, L[0]));
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(1, 2, ghr, L[1]));
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(2, 2, ghr, L[2]));
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(3, 2, ghr, L[3]));
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(4, 2, ghr, L[4]));
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(5, 2, ghr, L[5]));
    TEST_ASSERT_EQUAL_UINT32(2, BP_GetCntIdx(6, 2, ghr, L[6]));
}

/**
 * @brief Ensures addValToCounter works as expected
 * */
void test_addValToCounter(void)
{
    /*
     * Default counter has a length of 4 bits => it can take values from 7 to -8
     * */

    bpCounter_t counter = 0;

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 1 /* value */);
    TEST_ASSERT_EQUAL_UINT32(1, counter);

    counter = 0;

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -1 /* value */);
    TEST_ASSERT_EQUAL_UINT32(-1, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -1 /* value */);
    TEST_ASSERT_EQUAL_HEX32(-2, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -6 /* value */);
    TEST_ASSERT_EQUAL_HEX32(-8, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -1 /* value */);
    TEST_ASSERT_EQUAL_HEX32(-8, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 1 /* value */); /* Saturated negative */
    TEST_ASSERT_EQUAL_HEX32(-8, counter);

    counter = 0;

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 6 /* value */);
    TEST_ASSERT_EQUAL_HEX32(6, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 1 /* value */);
    TEST_ASSERT_EQUAL_HEX32(7, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 1 /* value */);
    TEST_ASSERT_EQUAL_HEX32(7, counter);

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -1 /* value */); /* Saturated positive */
    TEST_ASSERT_EQUAL_HEX32(7, counter);
}

/**
 * @brief Ensures getCntSaturation works as expected
 * */
void test_getCntSaturation(void)
{
    bpCounter_t counter = 0;

    TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(counter, DEFAULT_COUNTER_LEN));

    counter = 6;
    TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(counter, DEFAULT_COUNTER_LEN));

    counter = 7;
    TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_POSITIVE, getCntSaturation(counter, DEFAULT_COUNTER_LEN));

    counter = -1;
    TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(counter, DEFAULT_COUNTER_LEN));

    counter = -7;
    TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(counter, DEFAULT_COUNTER_LEN));

    counter = -8;
    TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_NEGATIVE, getCntSaturation(counter, DEFAULT_COUNTER_LEN));
}

/**
 * @brief Ensures BP_GetAliasingRatio works as expected
 * */
void test_BP_GetAliasingRatio_ColdStart(void)
{
    /*
     * Aliasing counter has a length of 9 bits => it can take values from 255 to -256
     * */
    {
        aliasingCounter = 254;

        TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(aliasingCounter, 9));

        aliasingCounter = 255;

        TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_POSITIVE, getCntSaturation(aliasingCounter, 9));

        aliasingCounter = -255;

        TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(aliasingCounter, 9));

        aliasingCounter = -256;

        TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_NEGATIVE, getCntSaturation(aliasingCounter, 9));
    }

    // BS 2 151
    TEST_ASSERT_EQUAL_HEX32(USE_SHORT_GHR, BP_GetAliasingRatio(1, 0, 2, 2));

    aliasingCounter = 0;
    addValToCounter(&aliasingCounter, sizeof(aliasingCounter), 9, -256);

    TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_NEGATIVE, getCntSaturation(aliasingCounter, 9));

    TEST_ASSERT_EQUAL_HEX32(USE_SHORT_GHR, BP_GetAliasingRatio(1, 0, 2, 2));

    aliasingCounter = 0;
    addValToCounter(&aliasingCounter, sizeof(aliasingCounter), 9, 255);

    TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_POSITIVE, getCntSaturation(aliasingCounter, 9));

    TEST_ASSERT_EQUAL_HEX32(USE_LONG_GHR, BP_GetAliasingRatio(1, 0, 2, 2));
}

/**
 * @brief Ensures BP_UpdateThreshold works as expected
 * */
void test_BP_UpdateThreshold(void)
{
    /*
     * threshold_counter counter has a length of 7 bits => it can take values from 63 to -64
     * */
    {
        threshold_counter = 62;

        TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(threshold_counter, 7));

        threshold_counter = 63;

        TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_POSITIVE, getCntSaturation(threshold_counter, 7));

        threshold_counter = -63;

        TEST_ASSERT_EQUAL_HEX32(0, getCntSaturation(threshold_counter, 7));

        threshold_counter = -64;

        TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_NEGATIVE, getCntSaturation(threshold_counter, 7));

        threshold_counter = 0;
    }

    /* Missprediction branch */
    {

        TEST_ASSERT_EQUAL_HEX32(7, theta_threshold);

        BP_UpdateThreshold(1, 0, 2);

        TEST_ASSERT_EQUAL_HEX32(1, threshold_counter);

        threshold_counter = 63;
        TEST_ASSERT_EQUAL_HEX32(CNT_SATURATED_POSITIVE, getCntSaturation(threshold_counter, 7));

        BP_UpdateThreshold(1, 0, 2);

        TEST_ASSERT_EQUAL_HEX32(0, threshold_counter);
        TEST_ASSERT_EQUAL_HEX32(8, theta_threshold);
    }
}

void test_BP_GetPrediction(void)
{
    FILE* file;
    char cwd[400];
    char line[24];
    uint32_t i       = 1;
    bool realOutcome = 0;
    bool prediction  = 0;
    int currentPc    = 0;
    int nextPc       = 0;
    int sum          = 0;
    int takenCount   = 0;
    int predictedCount = 0;
    int miss = 0;

    getcwd(cwd, sizeof(cwd));

    strcat(cwd, "/input/FSORT.TRA");

    file = fopen(cwd, "rb");

    BP_Init();

    while (fgets(line, sizeof(line), file))
    {
        printf("%d: %s", i++, line);

        /* Parse line */
        {
            if (line[0] == 'B')
                realOutcome = 1;
            else if (line[0] == 'N')
                realOutcome = 0;
            else
                return;

            char* token = strtok(line, " ");
            token = strtok(NULL, " ");
            currentPc = atoi(token);
            token = strtok(NULL, " ");
            nextPc = atoi(token);
        }

        prediction = BP_GetPrediction(currentPc, &sum);
        BP_Update(realOutcome, prediction, currentPc, nextPc, sum);

        if (realOutcome != prediction)
            miss++;

        if (realOutcome)
            takenCount++;
        if (prediction)
            predictedCount++;

    }

    printf("%d %d %d\n",i, takenCount, predictedCount);
    printf("%d\n", miss);
    printf("Miss rate: %f\n", (float)miss / i);

    fclose(file);
}
