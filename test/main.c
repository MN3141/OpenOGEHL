/* ================================================ INCLUDES =============================================== */
#include "unity.h"
#include "bp_test.h"
#include "unity_internals.h"
/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
void setUp(void) { }

void tearDown(void) { }

/* ================================================ MODULE API ============================================= */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_BP_InitL);
    RUN_TEST(test_BP_GetCntIdx_GhrIsEmpty);
    RUN_TEST(test_addValToCounter);
    RUN_TEST(test_getCntSaturation);
    RUN_TEST(test_BP_GetAliasingRatio_ColdStart);

    return (UNITY_END());
}
