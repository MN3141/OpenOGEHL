#define RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
/* ================================================ INCLUDES =============================================== */
#include <stdio.h>
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
static GuiWindowFileDialogState file_dialog_handle;
static Color dark_background = GRAY;
static Color light_background = RAYWHITE;
static Color current_background;
static char trace_file_path[PATH_SIZE];
static char file_name[PATH_SIZE] = "Foo";
static char current_theme[6] = "Dark\0";
static int text_size = 20;
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
void InitUI()
{
    current_background = dark_background;
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

        bool loaded_file_label = GuiLabel((Rectangle){0.02f * winW, 0.1 * winH, 0.20f * winW, 0.10f * winH}, file_name);
        bool prediction_result_label = GuiLabel((Rectangle){0.02f * winW, 0.1 * winH + 0.10f * winH, 0.20f * winW, 0.10f * winH}, prediction_label);
        bool load_trace_clicked = GuiButton((Rectangle){0.80f * winW, 0.4f * winH, 0.20f * winW, 0.10f * winH}, "Load Trace");
        bool start_sim_clicked = GuiButton((Rectangle){0.80f * winW, 0.4f * winH + 0.10f * winH, 0.20f * winW, 0.10f * winH}, "Start simulation");
        bool switch_theme_clicked = GuiButton((Rectangle){0.80f * winW, 0.4f * winH - 0.10f * winH, 0.20f * winW, 0.10f * winH}, current_theme);

        if (load_trace_clicked)
            file_dialog_handle.windowActive = true;

        if (start_sim_clicked && strcmp(trace_file_path,"") != 0)
        {
            thread_com.simulation_started = 1;
            thread_com.file_path = trace_file_path;
        }

        if(switch_theme_clicked)
        {
            if(strcmp(current_theme,"Dark\0") == 0 )
            {
                strcpy(current_theme,"Light\0");
                GuiLoadStyleDefault();
                GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
                current_background = light_background;
            }
            else{
                strcpy(current_theme,"Dark\0");
                GuiLoadStyleDark();
                GuiSetStyle(DEFAULT, TEXT_SIZE, text_size);
                current_background = dark_background;
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
