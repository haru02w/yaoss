#pragma once
#include "../util/vector.h"

typedef struct process_data {
    char nome[20];
    int pid;
    int pc;
    int seg;
    int priority;
    int seg_size;
    struct vector semaphore;
    struct vector instruction;
} pdata_t;

pdata_t *program_init(const char *path);
void program_destroy(pdata_t *program);
