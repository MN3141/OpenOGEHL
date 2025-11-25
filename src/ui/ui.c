#define RAYGUI_IMPLEMENTATION
/* ================================================ INCLUDES =============================================== */
#include <stdio.h>
#include "ui.h"
/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
static char traces_folder[] = "../../traces";
static char files_list[]= {
        "FBUBBLE.TRA\nFMATRIX.TRA\nFPERM.TRA\nFPUZZLE.TRA\nFQUEENS.TRA\nFSORT.TRA\nFTOWER.TRA\nFTREE.TRA"
    };
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
/* ================================================ MODULE API ============================================= */
static int LoadTraces(){
    return 0;
}
void InitUI(){
    InitWindow(800, 450, "Open O-GEHL");

     while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int winW = GetScreenWidth();
        int winH = GetScreenHeight();
        bool load_trace_clicked = GuiButton((Rectangle){ 0, winH*0.6, winW*0.1, winH*0.62}, "Load Traces");
        bool start_sim_clicked = GuiButton((Rectangle){400,0,410,50}, "Start Simulation");
        int text_box = GuiTextBox((Rectangle){0, 50,200,200}, files_list, 10, 0);
        if(load_trace_clicked)
            {
                LoadTraces();
                TraceLog(LOG_INFO, "Loading traces...");
            }
        EndDrawing();
    }

    CloseWindow();
}