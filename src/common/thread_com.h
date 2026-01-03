#ifndef THREAD_COM_H
#define THREAD_COM_H

/* ================================================ INCLUDES =============================================== */
/* ================================================= MACROS ================================================ */
/* ======================================= TYPEDEFS, ENUMS, STRUCTS ======================================== */
typedef struct
{
    float prediction_result;
    char *file_path;
    int simulation_started;
    int table_num;
    int table_size;
    int counter_len;
} thread_com_t;
extern thread_com_t thread_com;
/* ============================================ INLINE FUNCTIONS =========================================== */
/* ======================================= EXTERN GLOBAL VARIABLES ========================================= */
/* =============================================== MODULE API ============================================== */

#endif /* THREAD_COM_H */