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

struct vector *get_processes_info(/* idk what you need */
    bool keep_running)
{
    static struct vector process_rows = {};
    if (process_rows.data_size == 0)
        process_rows = vector_create(sizeof(struct process_info));

    if (!keep_running) {
        vector_destroy(&process_rows);
        return NULL;
    }

    int length = 1;
    void *ptr
        = realloc(process_rows.data, length * sizeof(struct process_info));
    assert(ptr != NULL);

    process_rows.data = ptr;
    process_rows.length = length;

    srand(time(NULL));

    memcpy(process_rows.data,
        &(struct process_info) {
            .process_id = rand(),
            .name = "Name yay",
            .priority = rand() % 21,
            .program_counter = rand(),
            .instr_total = 10000,
            .segment_id = rand(),
            .time_elapsed_ut = rand(),
        },
        sizeof(struct process_info));
    return &process_rows;
}
