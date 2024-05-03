#ifndef PROCESS_H_
#define PROCESS_H_
#include <stdio.h>
#include <string.h>
#include "instruction.h"

typedef struct process_data {
    char nome[20];
	int pid;
    int seg;
    int priority;
	int seg_size;
	//TODO: lista semaphore
	//TODO: lista instruction

}pdata_t;

#endif // PROCESS_H_
