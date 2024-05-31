/**
 * @file kernel.h
 * @brief Kernel declarations and definitions.
 */

#pragma once

#include "../process/semaphore.h"
#include "memory.h"
#include "scheduler.h"
#include <pthread.h>

/**
 * @enum event_code
 * @brief Enumeration for different types of kernel events.
 */
enum event_code {
    PROCESS_INTERRUPT,
    PROCESS_CREATE,
    PROCESS_FINISH,
    MEM_LOAD_REQ,
    MEM_LOAD_FINISH,
    SEMAPHORE_P,
    SEMAPHORE_V
};

/**
 * @enum run_mode
 * @brief Enumeration for different run modes of the kernel.
 */
enum run_mode { DEFAULT, SKIP };

/**
 * @struct kernel
 * @brief Structure representing the kernel.
 */
struct kernel {
    struct vector process_table;
    struct segment_table seg_table;
    struct sched scheduler;
    struct vector semaphore_table;
    unsigned cur_process_time;
    pthread_t event_thread;
    bool event_thread_running;
    enum event_code cur_event;
    void *cur_data;
};

/**
 * @brief Initializes the kernel.
 */
void kernel_init();

/**
 * @brief Handles interrupt events in the kernel.
 * @param interrupt_code The code of the interrupt event.
 * @param extra_data Additional data for the interrupt event.
 */
void interrupt_control(enum event_code interrupt_code, void *extra_data);

/**
 * @brief Handles system call events in the kernel.
 * @param syscall_code The code of the system call event.
 * @param extra_data Additional data for the system call event.
 */
void syscall(enum event_code syscall_code, void *extra_data);

/**
 * @brief Adds a semaphore to the kernel's semaphore table.
 * @param semaphore The name of the semaphore.
 */
void semaphore_add(const char *semaphore);

/**
 * @brief Runs the kernel.
 */
void kernel_run();

/**
 * @brief Shuts down the kernel and performs cleanup.
 */
void kernel_shutdown();

/**
 * @brief Retrieves a process from the kernel's process table.
 * @param pid The process ID.
 * @return A pointer to the process data.
 */
pdata_t *kernel_get_process(size_t pid);

/**
 * @brief Gets the next available process ID.
 * @return The next available process ID.
 */
size_t get_next_pid();

/**
 * @brief Wakes up a process in the kernel.
 * @param pid The process ID.
 */
void wakeup(size_t pid);
