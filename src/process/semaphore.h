/**
 * @file semaphore.h
 * @brief Header file for semaphore-related declarations.
 */

#pragma once

#include "../util/list.h"
#include "process.h"

/**
 * @brief Represents a semaphore.
 */
struct semaphore {
    char name[10];
    unsigned int S;
    struct list *waiters;
    int handler_pid;
};

/**
 * @brief Initializes the semaphore table.
 * @return A newly initialized vector to hold semaphores.
 */
struct vector semaphore_table_init();

/**
 * @brief Registers a new semaphore in the semaphore table.
 * @param sem_table Pointer to the semaphore table.
 * @param sem The name of the semaphore to register.
 */
void semaphore_register(struct vector *sem_table, const char *sem);

/**
 * @brief Finds a semaphore in the semaphore table by its name.
 * @param sem_table Pointer to the semaphore table.
 * @param sem The name of the semaphore to find.
 * @return A pointer to the found semaphore, or NULL if not found.
 */
struct semaphore *semaphore_find(struct vector *sem_table, const char *sem);

/**
 * @brief Performs the P (wait) operation on a semaphore.
 * @param sem Pointer to the semaphore.
 * @param proc Pointer to the process performing the operation.
 */
void semaphore_P(struct semaphore *sem, pdata_t *proc);

/**
 * @brief Performs the V (signal) operation on a semaphore.
 * @param sem Pointer to the semaphore.
 */
void semaphore_V(struct semaphore *sem);

void semaphore_table_destroy(struct vector *sem_table);
