#pragma once
#include "../util/vector.h"
#define QT 5000 // basis quantum time

/// @brief enum for the status of a process
typedef enum { NEW, READY, BLOCKED, TERMINATED, RUNNING } status_t;

/// @brief struct for data of a process (control process block (CPB))
typedef struct process_data {
    // name of process
    char name[20];

    // id of process
    size_t pid;

    // program counter
    size_t pc;

    // segment identifier
    size_t seg_id;

    // size of segment
    size_t seg_size;

    // priority of a process
    unsigned priority;

    // quantum time for round robin scheduler
    unsigned quantum_time;

    // approximated remaining time to process end
    unsigned remaining_time;

    // actual status of process
    status_t status;

    // semaphore list of process
    struct vector semaphore;
} pdata_t;

/// @brief function that reads a program file in the directory
/// @param path directory to the program file
void program_init(const char *path);

/// @brief function to free the memory allocated for the program
/// @param program pointer to the program to be destroyed
void program_destroy(pdata_t *program);
