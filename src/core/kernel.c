#include "kernel.h"
#include "memory.h"

static void process_interrupt(void *extra_data) { }

static void process_create(void *extra_data) { }

static void process_finish(void *extra_data) { }

static void mem_load_req(void *extra_data)
{
    // mem_load_request(
    //     &kernel->segment_table, (struct memory_request *)extra_data);

    interrupt_control(MEM_LOAD_FINISH, extra_data);
}

static void mem_load_finish(void *extra_data)
{
    struct memory_request *request = (struct memory_request *)extra_data;

    // vector_push_back(&kernel->process_table, request->process);

    // TODO: Add new process to the scheduler
}

static void semaphore_p(void *extra_data) { }

static void semaphore_v(void *extra_data) { }

void (*kernel_event[])(void *) = { process_interrupt, process_create,
    process_finish, mem_load_req, mem_load_finish, semaphore_p, semaphore_v };

void kernel_init() { }

void syscall(enum event_code syscall_code, void *extra_data)
{
    kernel_event[syscall_code](extra_data);
}

void interrupt_control(enum event_code interrupt_code, void *extra_data)
{
    kernel_event[interrupt_code](extra_data);
}

void kernel_shutdown() { }
