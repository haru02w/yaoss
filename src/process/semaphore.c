#include "semaphore.h"
#include "../core/kernel.h"
#include "../util/list.h"
#include "../util/vector.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vector semaphore_table_init()
{
    return (struct vector) { .capacity = 0,
        .data = NULL,
        .data_size = sizeof(struct semaphore),
        .length = 0 };
}

struct semaphore *semaphore_init(const char *sem)
{
    struct semaphore *sema = malloc(sizeof(struct semaphore));
    strcpy(sema->name, sem);
    sema->S = 0;
    sema->waiters = list_create(NULL);
    return sema;
}

void semaphore_register(struct vector *sem_table, const char *sem)
{
    for (size_t i = 0; i < sem_table->length; i++) {
        if (strcmp(((struct semaphore *)vector_get(sem_table, i))->name, sem)) {
            printf("semafaro ja registrado\n");
            return;
        }
    }
    struct semaphore *sem_aux = semaphore_init(sem);
    vector_push_back(sem_table, (void *)sem_aux);
    free(sem_aux);
}

struct semaphore *semaphore_find(struct vector *sem_table, const char *sem)
{
    for (size_t i = 0; i < sem_table->length; i++) {
        if (strcmp(((struct semaphore *)vector_get(sem_table, i))->name, sem)) {
            return (struct semaphore *)vector_get(sem_table, i);
        }
    }
    printf("semaforo não encontrado\n");
    return NULL;
}

void semaphore_P(struct semaphore *sem, pdata_t *proc)
{
    if (sem->S == 0) {
        sem->S++;
        return;
    } else {
        sem->S++;
        enqueue(sem->waiters, (void *)proc);
        int aux = 1;
        syscall(PROCESS_INTERRUPT, &aux);
    }
}

void semaphore_V(struct semaphore *sem)
{
    sem->S--;
    if (sem->S > 0) {
        pdata_t *proc = dequeue(sem->waiters);
        wakeup(proc->pid);
    }
}
