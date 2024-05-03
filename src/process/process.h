#ifndef PROCESS_PROCESS_H_
#define PROCESS_PROCESS_H_
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "../util/list.h"

typedef struct process_data {
    char nome[20];
	int pid;
    int seg;
    int priority;
	int seg_size;
	list_t *semaphore;
	list_t *instruction;
}pdata_t;

pdata_t *program_init(char *path);

#endif // PROCESS_PROCESS_H_
