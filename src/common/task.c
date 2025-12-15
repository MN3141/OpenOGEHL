/* ================================================ INCLUDES =============================================== */
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "thread_com.h"
#include "task.h"
#include "bp.h"
/* ================================================= MACROS ================================================ */
/* ============================================ LOCAL VARIABLES ============================================ */
/* ============================================ GLOBAL VARIABLES =========================================== */
/* ======================================= LOCAL FUNCTION DECLARATIONS ===================================== */
static void *GetPredictionsTask(void *arg);
/* ======================================== LOCAL FUNCTION DEFINITIONS ===================================== */
static void *GetPredictionsTask(void *arg)
{
    char line_buffer[24];
    uint32_t i = 1;
    bool real_outcome = 0;
    bool prediction = 0;

    int current_pc = 0;
    int next_pc = 0;
    int sum = 0;

    int taken_count = 0;
    int predicted_count = 0;


    while (1)
    {
        while (!thread_com.simulation_started);

        FILE *file_handle = fopen(thread_com.file_path, "r");
        fseek(file_handle, 0, SEEK_END);
        uint32_t size = ftell(file_handle);
        rewind(file_handle);
        memset(line_buffer,0,sizeof(line_buffer));
        i = 1;

        real_outcome = 0;
        prediction = 0;
        current_pc = 0;

        next_pc = 0;
        sum = 0;

        BP_Init();

        if (!file_handle)
        {
            perror("Failed to open trace file");
            return NULL;
        }
        
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
        fclose(file_handle);

        if (predicted_count != 0)
            thread_com.prediction_result = (float)taken_count / predicted_count;
        else
            thread_com.prediction_result = 0.0;
        thread_com.simulation_started = 0;
    }
}
/* ================================================ MODULE API ============================================= */
void CreateTasks()
{

    pthread_t pred_task_handle;
    pthread_attr_t *thread_attrib = NULL;

    pthread_create(&pred_task_handle, thread_attrib, GetPredictionsTask, NULL);
}
