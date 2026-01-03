#define RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
/* ================================================ INCLUDES =============================================== */
#include <string.h>
#include "ui.h"
#include "raygui.h"
#include "gui_window_file_dialog.h"
#include "style_dark.h"
#include "thread_com.h"
/* ================================================= MACROS ================================================ */
#define PATH_SIZE 1000
#define RESULT_SIZE 13
#define INPUT_BUFFER_SIZE 50
#define HELPER_TEXT_SIZE 100
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
static GuiWindowFileDialogState file_dialog_handle;
static Color dark_background = GRAY;
static Color light_background = RAYWHITE;
static Color current_background;

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

    WidgetSizeParameters buttonParameters = {
    .xFactor = 0.80f,
    .yFactor = 0.25f,
    .widthFactor = 0.20f,
    .heightFactor = 0.10f,
    .yStepFactor = 0.10f
    };
    WidgetSizeParameters labelParameters = {
    .xFactor = 0.02f,
    .yFactor = 0.1,
    .widthFactor = 0.20f,
    .heightFactor = 0.10f,
    .yStepFactor = 0.05f
    };
    WidgetSizeParameters inputBoxParameters = {
    .xFactor = 0.02f,
    .yFactor = 0.25f,
    .widthFactor = 0.60f,
    .heightFactor = 0.10f,
    .yStepFactor = 0.10f
    };
    WidgetSizeParameters helperText = {
        .xFactor = 0.02f,
        .yFactor = 0.5f,
        .widthFactor = 0.4f,
        .heightFactor = 0.3f,
        .yStepFactor = 0.1f
    };
    char trace_file_path[PATH_SIZE];
    char file_name[PATH_SIZE] = "Foo";
    char themes[THEMES_NUM][MAX_THEME_NAME_SIZE] =
        {
            "Dark Mode",
            "Light Mode"
        };

    const char numOfTablesText[] = "Enter number of tables (4 - 12 tables):";
    const char tableSizeText[] = "Enter size for a table in KB (256 - 2048 B range):";
    const char counterLenText[] = "Enter number of bits used for the counters (3 - 5 bits):";

    char numOfTablesBuffer[INPUT_BUFFER_SIZE];
    char tableSizeBuffer[INPUT_BUFFER_SIZE];
    char counterLenBuffer[INPUT_BUFFER_SIZE];

    bool inputEditMode[INPUT_NUM] = {false,false,false};
    bool userInput[INPUT_NUM] = {false,false,false};

    strcpy(numOfTablesBuffer,numOfTablesText);
    strcpy(tableSizeBuffer,tableSizeText);
    strcpy(counterLenBuffer,counterLenText);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(current_background);

        int winW = GetScreenWidth();
        int winH = GetScreenHeight();

        char prediction_label[RESULT_SIZE];
        sprintf(prediction_label, "Rate: %f", thread_com.prediction_result);

        /* ======================================= Labels ======================================= */
        GuiLabel((Rectangle){labelParameters.xFactor* winW, labelParameters.yFactor* winH,
            labelParameters.widthFactor* winW, labelParameters.heightFactor* winH},
            file_name);

        GuiLabel((Rectangle){labelParameters.xFactor* winW, labelParameters.yFactor* winH + labelParameters.yStepFactor*winH,
            labelParameters.widthFactor* winW, labelParameters.heightFactor* winH},
            prediction_label);
        /* ======================================= Labels ======================================= */

        /* ======================================= Buttons ======================================= */
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

        /* ======================================= Buttons ======================================= */

        /* ======================================= User input ======================================= */
        userInput[TABLE_NUM] = GuiTextBox((Rectangle)
            {inputBoxParameters.xFactor* winW, inputBoxParameters.yFactor* winH,
            inputBoxParameters.widthFactor* winW, inputBoxParameters.heightFactor* winH},
            numOfTablesBuffer, INPUT_BUFFER_SIZE, inputEditMode[TABLE_NUM]);

        userInput[TABLE_SIZE] = GuiTextBox((Rectangle)
           {inputBoxParameters.xFactor* winW, inputBoxParameters.yFactor* winH + inputBoxParameters.yStepFactor * winH,
            inputBoxParameters.widthFactor* winW, inputBoxParameters.heightFactor* winH},
            tableSizeBuffer, INPUT_BUFFER_SIZE, inputEditMode[TABLE_SIZE]);

        userInput[COUNTER_LEN] = GuiTextBox((Rectangle)
           {inputBoxParameters.xFactor* winW, inputBoxParameters.yFactor* winH + 2 * inputBoxParameters.yStepFactor * winH,
            inputBoxParameters.widthFactor* winW, inputBoxParameters.heightFactor* winH},
            counterLenBuffer, INPUT_BUFFER_SIZE, inputEditMode[COUNTER_LEN]);
        /* ======================================= User input ======================================= */

        /* ======================================= Helper text ======================================= */
        char helperTextBuffer[HELPER_TEXT_SIZE];
        sprintf(helperTextBuffer,
            "Current number of tables: %d\nCurrent table size: %d B\nCurrent counter size: %d bits",
            thread_com.table_num, thread_com.table_size, thread_com.counter_len);

        GuiLabel((Rectangle)
            {helperText.xFactor*winW, helperText.yFactor* winH,
            helperText.widthFactor* winW, helperText.heightFactor* winH},
            helperTextBuffer);

        /* ======================================= Helper text ======================================= */

        /* ======================================= Widget logic ======================================= */
        if(userInput[TABLE_NUM])
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                int tableNum = atoi(numOfTablesBuffer);
                if(tableNum < 4 || tableNum > 12)
                    strcpy(numOfTablesBuffer,"Please select a value between 4 and 12.");
                else
                    thread_com.table_num = tableNum;
            }
            else
                strcpy(numOfTablesBuffer,numOfTablesText);
            inputEditMode[TABLE_NUM] = !inputEditMode[TABLE_NUM];
        }
        if(userInput[TABLE_SIZE])
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                int tableSize = atoi(tableSizeBuffer);
                if(tableSize < 256 || tableSize > 2048)
                    strcpy(tableSizeBuffer,"Please select a value between 258 and 2048 B.");
                else
                    thread_com.table_size = tableSize;
            }
            else
                strcpy(tableSizeBuffer,tableSizeText);
            inputEditMode[TABLE_SIZE] = !inputEditMode[TABLE_SIZE];
        }
        if(userInput[COUNTER_LEN])
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                int counterLen = atoi(counterLenBuffer);
                if(counterLen < 3 || counterLen > 5)
                    strcpy(counterLenBuffer,"Please select a value 3 and 5.");
                else
                    thread_com.counter_len = counterLen;
            }
            else
                strcpy(counterLenBuffer,counterLenText);
            inputEditMode[COUNTER_LEN] = !inputEditMode[COUNTER_LEN];
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

        if (load_trace_clicked)
            file_dialog_handle.windowActive = true;

        if (start_sim_clicked && strcmp(trace_file_path,"") != 0)
        {
            thread_com.simulation_started = 1;
            thread_com.file_path = trace_file_path;
        }

        /* ======================================= Widget logic ======================================= */
        EndDrawing();
    }
    CloseWindow();
}
