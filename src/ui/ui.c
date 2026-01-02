#define RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
/* ================================================ INCLUDES =============================================== */
#include <string.h>
#include "ui.h"
#include "raygui.h"
#include "gui_window_file_dialog.h"
#include "style_dark.h"
#include "thread_com.h"
#include "bp.h"
/* ================================================= MACROS ================================================ */
#define PATH_SIZE 1000
#define RESULT_SIZE 13
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
static WidgetSizeParameters buttonParameters = {
    .xFactor = 0.80f,
    .yFactor = 0.4f,
    .widthFactor = 0.20f,
    .heightFactor = 0.10f,
    .yStepFactor = 0.10f
};
static WidgetSizeParameters labelParameters = {
    .xFactor = 0.02f,
    .yFactor = 0.1,
    .widthFactor = 0.20f,
    .heightFactor = 0.10f,
    .yStepFactor = 0.10f
};

static GuiWindowFileDialogState file_dialog_handle;
static Color dark_background = GRAY;
static Color light_background = RAYWHITE;
static Color current_background;

static char trace_file_path[PATH_SIZE];
static char file_name[PATH_SIZE] = "Foo";
static char themes[THEMES_NUM][MAX_THEME_NAME_SIZE] =
    {
        "Dark Mode",
        "Light Mode"
    };

static Theme currentTheme = DARK_THEME;
static int text_size = 20;
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
void InitUI()
{
    current_background = dark_background;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); /* flag necessary for making windows on Xfce resizable*/
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Open O-GEHL");

    GuiLoadStyleDark();
    GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
    file_dialog_handle = InitGuiWindowFileDialog(GetWorkingDirectory());
}
void UILoop()
{

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(current_background);

        int winW = GetScreenWidth();
        int winH = GetScreenHeight();

        char prediction_label[RESULT_SIZE];
        sprintf(prediction_label, "Rate: %f", thread_com.prediction_result);

        GuiLabel((Rectangle){labelParameters.xFactor* winW, labelParameters.yFactor* winH,
            labelParameters.widthFactor* winW, labelParameters.heightFactor* winH},
            file_name);

        GuiLabel((Rectangle){labelParameters.xFactor* winW, labelParameters.yFactor* winH + labelParameters.yStepFactor*winH,
            labelParameters.widthFactor* winW, labelParameters.heightFactor* winH},
            prediction_label);

        bool switch_theme_clicked = GuiButton((Rectangle)
            {buttonParameters.xFactor * winW, buttonParameters.yFactor * winH,
            buttonParameters.widthFactor * winW, buttonParameters.heightFactor * winH},
            themes[currentTheme]);

        bool load_trace_clicked = GuiButton((Rectangle)
            {buttonParameters.xFactor* winW, buttonParameters.yFactor* winH + buttonParameters.yStepFactor*winH,
            buttonParameters.widthFactor* winW, buttonParameters.heightFactor* winH},
            "Load Trace");

        bool start_sim_clicked = GuiButton((Rectangle)
            {buttonParameters.xFactor* winW, buttonParameters.yFactor* winH + 2* buttonParameters.yStepFactor*winH,
            buttonParameters.widthFactor* winW, buttonParameters.heightFactor* winH},
            "Start simulation");

        if (load_trace_clicked)
            file_dialog_handle.windowActive = true;

        if (start_sim_clicked && strcmp(trace_file_path,"") != 0)
        {
            thread_com.simulation_started = 1;
            thread_com.file_path = trace_file_path;
        }

        if(switch_theme_clicked)
        {
            switch (currentTheme)
            {
                case DARK_THEME:
                    currentTheme = LIGHT_THEME;
                    GuiLoadStyleDefault();
                    GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
                    current_background = light_background;
                    break;

                case LIGHT_THEME:
                    currentTheme = DARK_THEME;
                    GuiLoadStyleDark();
                    GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
                    current_background = dark_background;
                    break;
            }

        }
        GuiWindowFileDialog(&file_dialog_handle);

        if (file_dialog_handle.SelectFilePressed && IsFileExtension(file_dialog_handle.fileNameText, ".TRA"))
        {
            /*get aboslute path of selected file*/
            strcpy(trace_file_path, TextFormat("%s" PATH_SEPERATOR "%s", file_dialog_handle.dirPathText, file_dialog_handle.fileNameText));
            strncpy(file_name, GetFileName(trace_file_path), PATH_SIZE - 1);

            file_name[PATH_SIZE - 1] = '\0';
            TraceLog(LOG_INFO, "FILE SELECTED");

            file_dialog_handle.SelectFilePressed = false;
            file_dialog_handle.windowActive = false;
        }

        EndDrawing();
    }
    CloseWindow();
}
