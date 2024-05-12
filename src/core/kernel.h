#pragma once

#include "../process/semaphore.h"
#include "memory.h"
#include "scheduler.h"

enum event_code {
    PROCESS_INTERRUPT,
    PROCESS_CREATE,
    PROCESS_FINISH,

    MEM_LOAD_REQ,
    MEM_LOAD_FINISH,

    SEMAPHORE_P,
    SEMAPHORE_V
};

struct kernel {
    struct vector process_table;
    struct segment_table seg_table;
    struct sched scheduler;
    struct vector semaphore_table;
    unsigned cur_process_time;
};

void kernel_init();
void interrupt_control(enum event_code interrupt_code, void *extra_data);
void syscall(enum event_code event, void *extra_data);
void kernel_run();
void kernel_shutdown();
size_t get_next_pid();
void wakeup(size_t pid);
