/* ================================================ INCLUDES =============================================== */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bp.h"
#include "ui.h"
#include "thread_com.h"
/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
void *GetPredictionsTask(void *arg);
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
void *GetPredictionsTask(void *arg)
{

    while (!thread_com.simulation_started);

    FILE *file_handle = fopen(thread_com.file_path, "r");

    if (!file_handle) {
        perror("Failed to open trace file");
        return NULL;
    }

    uint32_t i = 1;
    bool real_outcome = 0;
    bool prediction = 0;
    int current_pc = 0;
    int next_pc = 0;
    int sum = 0;
    int taken_count = 0;
    int predicted_count = 0;
    char line_buffer[24];

    while (fgets(line_buffer, sizeof(line_buffer), file_handle))
    {
        if (line_buffer[0] == 'B')
            real_outcome = 1;
        else if (line_buffer[0] == 'N')
            real_outcome = 0;

        char *token = strtok(line_buffer, " ");
        token = strtok(NULL, " ");
        current_pc = atoi(token);
        token = strtok(NULL, " ");

        next_pc = atoi(token);
        prediction = BP_GetPrediction(current_pc, &sum);
        BP_Update(real_outcome, prediction, current_pc, next_pc, sum);

        if (real_outcome)
            taken_count++;
        if (prediction)
            predicted_count++;
    }

    if (predicted_count != 0)
        thread_com.prediction_result = (float)taken_count / predicted_count;
    else
        thread_com.prediction_result = 0.0;

}
/* ================================================ MODULE API ============================================= */
int main(int argc, char **argv)
{
    pthread_t taskHandle;
    pthread_attr_t *threadAttribute = NULL;
    int32_t sum;
    bool predictedOutcome = 0;

    InitUI();
    BP_Init();
    pthread_create(&taskHandle, threadAttribute, GetPredictionsTask, NULL);
    UILoop();
    predictedOutcome = BP_GetPrediction(123, &sum);

    BP_Update(1, predictedOutcome, 120, 124, sum);

    return 0;
}
