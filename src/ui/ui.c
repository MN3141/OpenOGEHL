#define RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
/* ================================================ INCLUDES =============================================== */
#include <stdio.h>
#include "ui.h"
#include "raygui.h"
#include "gui_window_file_dialog.h"
/* ================================================= MACROS ================================================ */
#define PATH_SIZE 1000
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
static GuiWindowFileDialogState file_dialog_handle;
static char trace_file_path[PATH_SIZE];
static int start_sim_event;
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
void InitUI()
{
    InitWindow(800, 450, "Open O-GEHL");
    file_dialog_handle = InitGuiWindowFileDialog(GetWorkingDirectory());
    start_sim_event = 0;
}
void UILoop()
{

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int winW = GetScreenWidth();
        int winH = GetScreenHeight();
        bool load_trace_clicked = GuiButton((Rectangle){0, winH * 0.6, winW * 0.1, winH * 0.62}, "Load Trace");
        bool start_sim_clicked = GuiButton((Rectangle){100, 100, 300, 300}, "Start simulation");

        if (load_trace_clicked)
        {
            file_dialog_handle.windowActive = true;
            TraceLog(LOG_INFO, "Loading traces...");
        }

        if (start_sim_clicked)
        {
            start_sim_event = 1;
            TraceLog(LOG_INFO, "Starting simulation...");
        }
        GuiWindowFileDialog(&file_dialog_handle);

        if (file_dialog_handle.SelectFilePressed)
        {
            /*get aboslute path of selected file*/
            strcpy(trace_file_path, TextFormat("%s" PATH_SEPERATOR "%s", file_dialog_handle.dirPathText, file_dialog_handle.fileNameText));
            TraceLog(LOG_INFO, "FILE SELECTED");
        }
        EndDrawing();
    }
    CloseWindow();
}