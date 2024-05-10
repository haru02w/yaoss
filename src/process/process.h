#pragma once
#include "../util/vector.h"
#define QT 5000

typedef enum { NEW, READY, BLOCKED, DEAD, RUNNING } status_t;

typedef struct process_data {
    char name[20];
    size_t pid;
    size_t pc;
    size_t seg_id;
    size_t seg_size;
    unsigned priority;
    unsigned quantum_time;
    unsigned remainig_time;
    status_t status;
    struct vector semaphore;
} pdata_t;

void program_init(const char *path);
void program_destroy(pdata_t *program);
