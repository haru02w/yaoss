#include "kernel.h"
#include <math.h>

#define TIME_PER_CALL 1000

// TODO: move
struct kernel kernel;

static void process_interrupt(void *extra_data)
{
    int blocked = *(int *)extra_data;

    if (blocked) {
        sched_process_block(&kernel.scheduler);
    } else {
        sched_next_process(&kernel.scheduler);
    }

    kernel.cur_process_time = 0;
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
    vector_push_back(&kernel.process_table, &(request->process));
    sched_add_process(&kernel.scheduler, request->process);
}

static void semaphore_p(void *extra_data)
{
    semaphore_P((struct semaphore *)extra_data, kernel.scheduler.atual);
}

static void semaphore_v(void *extra_data)
{
    semaphore_V((struct semaphore *)extra_data);
}

void (*kernel_event[])(void *) = { process_interrupt, process_create,
    process_finish, mem_load_req, mem_load_finish, semaphore_p, semaphore_v };

void kernel_init()
{
    kernel.process_table = vector_create(sizeof(pdata_t *));
    segment_table_init(&kernel.seg_table);
    scheduler_init(&kernel.scheduler);
    kernel.semaphore_table = semaphore_table_init();
}

void syscall(enum event_code syscall_code, void *extra_data)
{
    kernel_event[syscall_code](extra_data);
}

void interrupt_control(enum event_code interrupt_code, void *extra_data)
{
    kernel_event[interrupt_code](extra_data);
}

// TODO: read, write, print (works like exec for now) (change for a function
// pointer array)
static void exec_instruction(pdata_t *process, instruction_t *instruction)
{
    unsigned int max_exec_time
        = fmin(process->quantum_time - kernel.cur_process_time, TIME_PER_CALL);

    switch (instruction->op) {
    case READ:
    case WRITE:
    case PRINT:
    case EXEC:
        if (instruction->value > max_exec_time) {
            instruction->value -= max_exec_time;
            process->remaining_time -= max_exec_time;
            kernel.cur_process_time += max_exec_time;
        } else {
            instruction->value = 0;
            process->remaining_time -= instruction->value;
            kernel.cur_process_time += instruction->value;
            process->pc++;
        }
        break;
    case P:
        syscall(SEMAPHORE_P,
            semaphore_find(&kernel.semaphore_table, instruction->sem));
        if (process->status != BLOCKED) {
            process->remaining_time -= 200;
            kernel.cur_process_time += max_exec_time;
        }
        break;
    case V:
        syscall(SEMAPHORE_V,
            semaphore_find(&kernel.semaphore_table, instruction->sem));
        process->remaining_time -= 200;
        kernel.cur_process_time += max_exec_time;
        break;
    default:;
    }
}

// TODO: kernel main function
void kernel_run()
{
    pdata_t *process = kernel.scheduler.atual;

    if (process == NULL)
        return;

    if (kernel.cur_process_time >= process->quantum_time) {
        syscall(PROCESS_INTERRUPT, process);
        return;
    }

    instruction_t *instruction = segment_fetch_instruction(
        &kernel.seg_table, process->seg_id, process->pc);

    exec_instruction(process, instruction);

    if (process->remaining_time == 0) {
        syscall(PROCESS_FINISH, process);
    }
}

void kernel_shutdown()
{
    for (size_t i = 0; i < kernel.process_table.length; i++) {
        pdata_t *cur_proc = vector_get(&kernel.process_table, i);
        program_destroy(cur_proc);
    }

    vector_destroy(&kernel.process_table);
    segment_table_destroy(&kernel.seg_table);
    // TODO: scheduler_destroy()
    // TODO: semaphore_table_destroy()
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
