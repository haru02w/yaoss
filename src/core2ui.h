#pragma once
#include <stdbool.h>
#include <stdint.h>

struct simulation_info {
    uint64_t memory_usage_mb;
};
struct simulation_info get_simulation_info(/* idk what you need */);

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
};

struct vector * /* of struct process_info */ get_processes_info(
    /* idk what you need */);

// TODO: get_process_details()
