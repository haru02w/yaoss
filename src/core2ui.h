#pragma once
#include "util/vector.h"
#include <stdbool.h>
#include <stdint.h>

struct sim_info {
    uint64_t memory_usage_mb;
};

//***

struct process_info {
    uint16_t process_id;
    char *name;
    uint16_t priority;
    char process_state;
    uint64_t program_counter;
    uint64_t instr_total;
    uint16_t segment_id;
    uint64_t memory_usage_kb;
    uint64_t time_elapsed_ut;
    char operation[4 + 1];
    char operation_value[6 + 1];
};

struct semaphore_info {
    char name[4 + 1];
    uint16_t working_process_id;
    uint64_t waiting_counter;
};

struct page_info {
    uint16_t page_id;
    bool using;
    bool on_disk;
};

void get_simulation_info(struct sim_info *sim_info);
void get_proc_info(struct vector *proc_info /* struct process_info */);
void get_sem_info(
    struct vector *sem_info /* struct semaphore_info */, uint16_t pid);
void get_page_info(
    struct vector *page_info /* struct page_info */, uint16_t pid);

// TODO: get_process_details()
