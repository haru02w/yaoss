#include "core2ui.h"
#include "util/vector.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct simulation_info get_simulation_info(/* idk what you need */)
{
    srand(time(NULL));
    return (struct simulation_info) {
        .memory_usage_mb = rand() % 1024,
    };
}

struct vector *get_processes_info(/* idk what you need */)
{
    static struct vector process_rows = {};
    if (!process_rows.data_size)
        process_rows = vector_create(sizeof(struct process_info));

    process_rows.length = 3;
    void *ptr = realloc(
        process_rows.data, process_rows.length * sizeof(struct process_info));
    assert(ptr != NULL);
    process_rows.data = ptr;

    srand(time(NULL));
    for (size_t i = 0; i < process_rows.length; i++)
        memcpy(&process_rows.data[i * sizeof(struct process_info)],
            &(struct process_info) {
                .process_id = rand(),
                .name = "Name yay",
                .priority = rand() % 21,
                .process_state = 'R',
                .program_counter = rand(),
                .instr_total = 10000,
                .segment_id = rand(),
                .memory_usage_kb = rand(),
                .time_elapsed_ut = rand(),
            },
            sizeof(struct process_info));
    return &process_rows;
}
