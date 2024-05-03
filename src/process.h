#ifndef PROCESS_H_
#define PROCESS_H_
#include<stdio.h>

typedef struct process_data {
    char nome[10];
    int seg;
    int priority;
	int seg_size;
	//TODO: lista semafore
	//TODO: lista instruction

};

#endif // PROCESS_H_
