/**
 * @file semaphore.c
 * @brief Implementation of semaphore functions.
 */

#include "semaphore.h"
#include "../core/kernel.h"
#include "../util/list.h"
#include "../util/vector.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes the semaphore table.
 * @return A newly initialized vector to hold semaphores.
 */
struct vector semaphore_table_init()
{
    return (struct vector) { .capacity = 0,
        .data = NULL,
        .data_size = sizeof(struct semaphore),
        .length = 0 };
}

/**
 * @brief Initializes a semaphore with a given name.
 * @param sem The name of the semaphore.
 * @return A pointer to the newly initialized semaphore.
 */
struct semaphore *semaphore_init(const char *sem)
{
    struct semaphore *sema = malloc(sizeof(struct semaphore));
    strcpy(sema->name, sem);
    sema->S = 0;
    sema->waiters = list_create(NULL);
    sema->handler_pid = -1;
    return sema;
}

/**
 * @brief Registers a new semaphore in the semaphore table.
 * @param sem_table Pointer to the semaphore table.
 * @param sem The name of the semaphore to register.
 */
void semaphore_register(struct vector *sem_table, const char *sem)
{
    for (size_t i = 0; i < sem_table->length; i++) {
        if (strcmp(((struct semaphore *)vector_get(sem_table, i))->name, sem)
            == 0) {
            return;
        }
    }
    struct semaphore *sem_aux = semaphore_init(sem);
    vector_push_back(sem_table, (void *)sem_aux);
    free(sem_aux);
}

/**
 * @brief Finds a semaphore in the semaphore table by its name.
 * @param sem_table Pointer to the semaphore table.
 * @param sem The name of the semaphore to find.
 * @return A pointer to the found semaphore, or NULL if not found.
 */
struct semaphore *semaphore_find(struct vector *sem_table, const char *sem)
{
    for (size_t i = 0; i < sem_table->length; i++) {
        if (strcmp(((struct semaphore *)vector_get(sem_table, i))->name, sem)
            == 0) {
            return (struct semaphore *)vector_get(sem_table, i);
        }
    }
    return NULL;
}

/**
 * @brief Performs the P (wait) operation on a semaphore.
 * @param sem Pointer to the semaphore.
 * @param proc Pointer to the process performing the operation.
 */
void semaphore_P(struct semaphore *sem, pdata_t *proc)
{
    if (sem->S == 0) {
        sem->S++;
        sem->handler_pid = proc->pid;
        return;
    } else {
        sem->S++;
        enqueue(sem->waiters, (void *)proc);
        int aux = 1;
        syscall(PROCESS_INTERRUPT, &aux);
    }
}

/**
 * @brief Performs the V (signal) operation on a semaphore.
 * @param sem Pointer to the semaphore.
 */
void semaphore_V(struct semaphore *sem)
{
    sem->S--;
    if (sem->S > 0) {
        pdata_t *proc = dequeue(sem->waiters);
        wakeup(proc->pid);
        sem->handler_pid = proc->pid;
    } else {
        sem->handler_pid = -1;
    }
}

void semaphore_table_destroy(struct vector *sem_table)
{
    for (size_t i = 0; i < sem_table->length; i++) {
        struct semaphore *sem = vector_get(sem_table, i);
        list_destroy(sem->waiters);
    }
    vector_destroy(sem_table);
}
