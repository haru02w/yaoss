#pragma once

#include "../util/vector.h"

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
};

void kernel_init();
void interrupt_control(enum event_code interrupt_code, void *extra_data);
void syscall(enum event_code event, void *extra_data);
void kernel_shutdown();
