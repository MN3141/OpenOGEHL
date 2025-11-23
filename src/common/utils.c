/* ================================================ INCLUDES =============================================== */
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
uint32_t getMask(uint32_t val)
{
    return (~0 << val) ^ ~0;
}

uint32_t getCntSaturation(uint32_t counterVal, size_t counterLen)
{
    if (counterVal == getMask(counterLen - 1))
        return CNT_SATURATED_POSITIVE;

    if (counterVal == -(1 << (counterLen - 1)))
        return CNT_SATURATED_NEGATIVE;

    return 0;
}

void addValToCounter(void* counter, size_t counterSize, size_t counterLen, int32_t val)
{
    int64_t newVal = 0;

    memcpy(&newVal, counter, counterSize);

    uint32_t saturation = getCntSaturation(newVal, counterLen);

    if ((CNT_SATURATED_POSITIVE == saturation) || (CNT_SATURATED_NEGATIVE == saturation))
        return;

    newVal += val;

    int64_t maxVal = getMask(counterLen - 1);
    int64_t minVal = -(1 << (counterLen - 1));

    if (newVal > maxVal || newVal < minVal)
        return;

    memcpy(counter, &newVal, counterSize);
}
