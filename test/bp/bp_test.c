/* ================================================ INCLUDES =============================================== */
#include "bp_test.h"
#include "bp.h"
#include "bp_defines.h"
#include "bp_types.h"
#include "unity.h"
#include "utils.h"
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
extern void BP_UpdateThreshold(bool realOutcome, bool predictedOutcome);
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

    TEST_ASSERT_EQUAL_UINT32(1,    L[0]); /* TODO: Conclude if the first number needs to be 0 or 1.
                                           * Using the formula that the paper provides, generates 1
                                           * for the first item, but they mention it to be 0. */
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
    bpCounter_t counter = 0;

    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, 1 /* value */);
    TEST_ASSERT_EQUAL_UINT32(1, counter);

    counter = 0;

    /* TODO: Find out if the counter has to be signed or unsigned */
    addValToCounter(&counter, sizeof(bpCounter_t), DEFAULT_COUNTER_LEN, -1 /* value */);
    TEST_ASSERT_EQUAL_UINT32(-1, counter);
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
}
