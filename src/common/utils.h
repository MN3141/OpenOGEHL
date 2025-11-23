#ifndef UTILS
#define UTILS

/* ================================================ INCLUDES =============================================== */
#include <stddef.h>
#include <stdint.h>

/* ================================================= MACROS ================================================ */
// #define LOG_EN
#ifdef LOG_EN
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) do { } while(0)
#endif

#define CNT_SATURATED_POSITIVE (0X3D3D3D3DUL)
#define CNT_SATURATED_NEGATIVE (0xD3D3D3D3UL)

/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* =============================================== MODULE API ============================================== */
uint32_t getMask(uint32_t val);
uint32_t getCntSaturation(uint32_t counterVal, size_t counterLen);
void addValToCounter(void* counter, size_t counterSize, size_t counterLen, int32_t val);

#endif /* UTILS */
