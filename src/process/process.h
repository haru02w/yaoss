#pragma once
#include "../util/list.h"

typedef struct process_data {
    char nome[20];
    int pid;
    int seg;
    int priority;
    int seg_size;
    struct list *semaphore;
    struct list *instruction;
} pdata_t;

pdata_t *program_init(const char *path);
