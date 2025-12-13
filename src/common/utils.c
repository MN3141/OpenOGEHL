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

void int32SafeCopy(int32_t* dest, void* source, size_t size)
{
    switch (size)
    {
        case sizeof(int8_t):
            *dest = *(int8_t*)source;
            return;
        case sizeof(int16_t):
            *dest = *(int16_t*)source;
            return;
        case sizeof(int32_t):
            *dest = *(int32_t*)source;
            return;
        case sizeof(int64_t):
            *dest = *(int64_t*)source;
            return;
        default:
            return;
    }
}

/**
* Description: Adds a signed value to a counter of a variable length of bits
*/
void addValToCounter(void* counter, size_t counterSize, size_t counterLen, int32_t val)
{
    int32_t newVal = 0;

    int32SafeCopy(&newVal, counter, counterSize);

    uint32_t saturation = getCntSaturation(newVal, counterLen);

    if ((CNT_SATURATED_POSITIVE == saturation) || (CNT_SATURATED_NEGATIVE == saturation))
        return;

    newVal += val;

    int32_t maxVal = getMask(counterLen - 1);
    int32_t minVal = -(1 << (counterLen - 1));

    if (newVal > maxVal || newVal < minVal)
        return;

    memcpy(counter, &newVal, counterSize);
}
