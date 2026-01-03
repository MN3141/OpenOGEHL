#ifndef UI
#define UI
/* ================================================ INCLUDES =============================================== */
/* ================================================= MACROS ================================================ */
#define WIN_WIDTH 1200
#define WIN_HEIGHT 800
#define THEMES_NUM 2
#define MAX_THEME_NAME_SIZE 11
#define INPUT_NUM 3
/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
typedef enum
{
    DARK_THEME,
    LIGHT_THEME
} Theme;
typedef enum{
    NONE = -1,
    TABLE_NUM,
    TABLE_SIZE,
    COUNTER_LEN
} UserInput;
typedef struct
{
    float xFactor;
    float yFactor;
    float widthFactor;
    float heightFactor;
    float yStepFactor;
} WidgetSizeParameters;
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* =============================================== MODULE API ============================================== */

void InitUI();
void UILoop();
#endif /* UI */