#include "core2ui.h"
#include "util/vector.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void get_simulation_info(struct sim_info *sim_info)
{
    srand(time(NULL));
    sim_info->memory_usage_mb = rand() % 1024;
}

void get_proc_info(struct vector *proc_info)
{
    proc_info->length = 3;
    void *ptr = realloc(
        proc_info->data, proc_info->length * sizeof(struct process_info));
    assert(ptr != NULL);
    proc_info->data = ptr;

    srand(time(NULL));
    for (size_t i = 0; i < proc_info->length; i++)
        memcpy(&proc_info->data[i * sizeof(struct process_info)],
            &(struct process_info) {
                .process_id = rand(),
                .name = "Name yay",
                .priority = rand() % 21,
                .process_state = 'R',
                .program_counter = rand() % 999,
                .instr_total = rand() % 999,
                .segment_id = rand() % 999,
                .memory_usage_kb = rand() % 999999,
                .time_elapsed_ut = rand() % 999999,
                .operation = "print",
                .operation_value = "150",
            },
            sizeof(struct process_info));
}

void get_sem_info(struct vector *sem_info, uint16_t pid)
{
    // TODO: get semaphore information of @pid
    sem_info->length = 3;
    void *ptr = realloc(
        sem_info->data, sem_info->length * sizeof(struct semaphore_info));
    assert(ptr != NULL);
    sem_info->data = ptr;

    srand(time(NULL));
    for (size_t i = 0; i < sem_info->length; i++) {
        memcpy(&sem_info->data[i * sizeof(struct semaphore_info)],
            &(struct semaphore_info) {
                .name = "yay",
                .working_process_id = rand() % 999,
                .waiting_counter = rand() % 999,
            },
            sizeof(struct semaphore_info));
    }
}

void get_page_info(struct vector *page_info, uint16_t pid)
{
    // TODO: get page information of @pid
    page_info->length = 3;
    void *ptr = realloc(
        page_info->data, page_info->length * sizeof(struct page_info));
    assert(ptr != NULL);
    page_info->data = ptr;

    srand(time(NULL));
    for (size_t i = 0; i < page_info->length; i++) {
        memcpy(&page_info->data[i * sizeof(struct page_info)],
            &(struct page_info) {
                .page_id = rand() % 999,
                .using = rand() % 2,
                .on_disk = rand() % 2,
            },
            sizeof(struct page_info));
    }
}
