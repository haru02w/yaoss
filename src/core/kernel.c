#include "kernel.h"
#include "memory.h"

// TODO: move
struct kernel kernel;

static void process_interrupt(void *extra_data)
{
    // TODO: scheduler_run()
}

static void process_create(void *extra_data)
{
    program_init((char *)extra_data);
}

static void process_finish(void *extra_data)
{
    pdata_t *process = (pdata_t *)extra_data;
    segment_table_remove(&kernel.seg_table, process->seg_id);
    program_destroy(process);
    // TODO: vector_remove()
    // TODO: scheduler_remove()
}

static void mem_load_req(void *extra_data)
{
    mem_load_request(&kernel.seg_table, (struct memory_request *)extra_data);
    interrupt_control(MEM_LOAD_FINISH, extra_data);
}

static void mem_load_finish(void *extra_data)
{
    struct memory_request *request = (struct memory_request *)extra_data;
    request->process->status = READY;
    vector_push_back(&kernel.process_table, request->process);
    // TODO: scheduler_add()
}

static void semaphore_p(void *extra_data) { }

static void semaphore_v(void *extra_data) { }

void (*kernel_event[])(void *) = { process_interrupt, process_create,
    process_finish, mem_load_req, mem_load_finish, semaphore_p, semaphore_v };

void kernel_init()
{
    kernel.process_table = vector_create(sizeof(pdata_t));
    segment_table_init(&kernel.seg_table);
    // TODO: scheduler_init()
}

void syscall(enum event_code syscall_code, void *extra_data)
{
    kernel_event[syscall_code](extra_data);
}

void interrupt_control(enum event_code interrupt_code, void *extra_data)
{
    kernel_event[interrupt_code](extra_data);
}

// TODO: kernel main function
void kernel_run() { }

void kernel_shutdown()
{
    for (size_t i = 0; i < kernel.process_table.length; i++) {
        pdata_t *cur_proc = vector_get(&kernel.process_table, i);
        program_destroy(cur_proc);
    }

    vector_destroy(&kernel.process_table);
    segment_table_destroy(&kernel.seg_table);
    // TODO: scheduler_destroy()
}

size_t get_next_pid()
{
    if (kernel.process_table.length == 0)
        return 0;

    pdata_t *last_proc
        = vector_get(&kernel.process_table, kernel.process_table.length - 1);

    return last_proc->pid + 1;
}

void wakeup(size_t pid) { sched_unlock_process(&(kernel.scheduler), pid); }
