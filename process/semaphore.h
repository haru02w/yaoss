#pragma once

#include "../util/list.h"
#include "process.h"

struct semaphore {
    char name[10];
    unsigned int S;
    struct list *waiters;
};

struct vector semaphore_table_init();
void semaphore_register(struct vector *sem_table, const char *sem);
struct semaphore *semaphore_find(struct vector *sem_table, const char *sem);
void semaphore_P(struct semaphore *sem, pdata_t *proc);
void semaphore_V(struct semaphore *sem);
