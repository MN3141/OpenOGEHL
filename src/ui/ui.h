#ifndef UI
#define UI
/* ================================================ INCLUDES =============================================== */
/* ================================================= MACROS ================================================ */
#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define THEMES_NUM 2
#define MAX_THEME_NAME_SIZE 11
/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
typedef enum
{
    DARK_THEME,
    LIGHT_THEME
} Theme;
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