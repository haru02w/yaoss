#include "core2ui.h"
#include "core/kernel_acess.h"
#include "core/memory.h"
#include "util/vector.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void get_simulation_info(struct sim_info *sim_info)
{
    sim_info->memory_usage_kb
        = (MAX_MEMORY_SIZE - kernel.seg_table.remaining_memory) / KILOBYTE;
}

void get_proc_info(struct vector *proc_info)
{
    proc_info->length = kernel.process_table.length;
    if (proc_info->length == 0)
        return;
    void *ptr = realloc(
        proc_info->data, proc_info->length * sizeof(struct process_info));
    assert(ptr != NULL);
    proc_info->data = ptr;

    for (size_t i = 0; i < proc_info->length; i++) {
        pdata_t *process = kernel_get_process(i + 1);
        instruction_t *instruction = segment_fetch_instruction(
            &kernel.seg_table, process->seg_id, process->pc);

        struct process_info info = (struct process_info) {
            .process_id = process->pid,
            .name = process->name,
            .priority = process->priority,
            .process_state = process->status,
            .program_counter = process->pc,
            .instr_total = process->code_size,
            .segment_id = process->seg_id,
            .memory_usage_kb = process->seg_size,
            .time_elapsed_ut = process->maximum_time - process->remaining_time,
        };

        instruction_to_string(info.operation, instruction->show_opcode);
        snprintf(info.operation_value, sizeof info.operation_value, "%ld",
            instruction->value);

        memcpy(&proc_info->data[i * sizeof(struct process_info)], &info,
            sizeof(struct process_info));
    }
}

void get_sem_info(struct vector *sem_info, uint16_t pid)
{
    // TODO: get semaphore information of @pid
    pdata_t *process = kernel_get_process(pid + 1);
    if (process == NULL || process->semaphore.length == 0) {
        sem_info->length = 0;
        return;
    }
    sem_info->length = process->semaphore.length;
    void *ptr = realloc(
        sem_info->data, sem_info->length * sizeof(struct semaphore_info));
    assert(ptr != NULL);
    sem_info->data = ptr;

    for (size_t i = 0; i < sem_info->length; i++) {
        const char *semaphore_name
            = *(char **)vector_get(&process->semaphore, i);
        struct semaphore *semaphore
            = semaphore_find(&kernel.semaphore_table, semaphore_name);

        memcpy(&sem_info->data[i * sizeof(struct semaphore_info)],
            &(struct semaphore_info) {
                .name = semaphore->name,
                .working_process_id = semaphore->handler_pid,
                .waiting_counter = semaphore->S,
            },
            sizeof(struct semaphore_info));
    }
}

void get_page_info(struct vector *page_info, uint16_t pid)
{
    // TODO: get page information of @pid
    pdata_t *process = kernel_get_process(pid + 1);
    if (process == NULL) {
        page_info->length = 0;
        return;
    }
    struct segment *segment = vector_get(&kernel.seg_table.table,
        segment_table_search(&kernel.seg_table, process->seg_id));

    page_info->length = segment->page_table_size;
    void *ptr = realloc(
        page_info->data, page_info->length * sizeof(struct page_info));
    assert(ptr != NULL);
    page_info->data = ptr;

    for (size_t i = 0; i < page_info->length; i++) {
        memcpy(&page_info->data[i * sizeof(struct page_info)],
            &(struct page_info) {
                .page_id = segment->page_table[i]->id,
                .using = segment->page_table[i]->used_bit,
                .on_disk = segment->page_table[i]->on_disk,
            },
            sizeof(struct page_info));
    }
}
