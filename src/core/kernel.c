/**
 * @file kernel.c
 * @brief Kernel implementation with process and memory management.
 */

#include "curses.h"
#include "kernel_acess.h"
#include <math.h>

#define TIME_PER_CALL 1

/// Kernel structure instance
struct kernel kernel;

/**
 * @brief Handles process interrupts.
 *
 * @param extra_data Pointer to extra data (in this case, an integer indicating
 * if the process is blocked).
 */
static void process_interrupt(void *extra_data)
{
    int blocked = *(int *)extra_data;

    if (blocked) {
        sched_process_block(&kernel.scheduler);
    }

    sched_next_process(&kernel.scheduler);
    kernel.cur_process_time = 0;
}

/**
 * @brief Creates a new process.
 *
 * @param extra_data Pointer to extra data (in this case, the process name).
 */
static void process_create(void *extra_data)
{
    program_init((char *)extra_data);
}

/**
 * @brief Finishes a process.
 *
 * @param extra_data Pointer to the process data.
 */
static void process_finish(void *extra_data)
{
    pdata_t *process = (pdata_t *)extra_data;
    sched_remove(&kernel.scheduler);
    sched_next_process(&kernel.scheduler);
    segment_table_remove(&kernel.seg_table, process->seg_id);

    for (size_t i = 0; i < kernel.process_table.length; i++) {
        pdata_t *cur_proc = kernel_get_process(i + 1);
        if (process->pid == cur_proc->pid) {
            vector_remove(&kernel.process_table, i);
            break;
        }
    }

    program_destroy(process);
    kernel.cur_process_time = 0;
}

/**
 * @brief Handles memory load requests.
 *
 * @param extra_data Pointer to the memory request data.
 */
static void mem_load_req(void *extra_data)
{
    if (mem_load_request(
            &kernel.seg_table, (struct memory_request *)extra_data))
        interrupt_control(MEM_LOAD_FINISH, extra_data);
}

/**
 * @brief Finalizes the memory load process.
 *
 * @param extra_data Pointer to the memory request data.
 */
static void mem_load_finish(void *extra_data)
{
    struct memory_request *request = (struct memory_request *)extra_data;
    request->process->status = READY;
    vector_push_back(&kernel.process_table, &(request->process));
    sched_add_process(&kernel.scheduler, request->process);
}

/**
 * @brief Handles semaphore P operation.
 *
 * @param extra_data Pointer to the semaphore data.
 */
static void semaphore_p(void *extra_data)
{
    semaphore_P((struct semaphore *)extra_data, kernel.scheduler.atual);
}

/**
 * @brief Handles semaphore V operation.
 *
 * @param extra_data Pointer to the semaphore data.
 */
static void semaphore_v(void *extra_data)
{
    semaphore_V((struct semaphore *)extra_data);
}

/// Array of kernel event handlers
void (*kernel_event[])(void *) = { process_interrupt, process_create,
    process_finish, mem_load_req, mem_load_finish, semaphore_p, semaphore_v };

/**
 * @brief Initializes the kernel.
 */
void kernel_init()
{
    kernel.process_table = vector_create(sizeof(pdata_t *));
    segment_table_init(&kernel.seg_table);
    scheduler_init(&kernel.scheduler);
    kernel.semaphore_table = semaphore_table_init();
}

/**
 * @brief Handles system calls.
 *
 * @param syscall_code The syscall event code.
 * @param extra_data Pointer to extra data for the syscall.
 */
void syscall(enum event_code syscall_code, void *extra_data)
{
    kernel_event[syscall_code](extra_data);
}

/**
 * @brief Handles interrupt control.
 *
 * @param interrupt_code The interrupt event code.
 * @param extra_data Pointer to extra data for the interrupt.
 */
void interrupt_control(enum event_code interrupt_code, void *extra_data)
{
    kernel_event[interrupt_code](extra_data);
}

/**
 * @brief Adds a semaphore to the kernel.
 *
 * @param semaphore The name of the semaphore.
 */
void semaphore_add(const char *semaphore)
{
    semaphore_register(&kernel.semaphore_table, semaphore);
}

/**
 * @brief Executes an instruction.
 *
 * @param process Pointer to the process data.
 * @param instruction Pointer to the instruction data.
 * @param mode The run mode (DEFAULT or other).
 */
static void exec_instruction(
    pdata_t *process, instruction_t *instruction, enum run_mode mode)
{
    unsigned int max_exec_time;

    if (mode == DEFAULT) {
        max_exec_time = fmin(
            process->quantum_time - kernel.cur_process_time, TIME_PER_CALL);
    } else {
        max_exec_time = process->quantum_time - kernel.cur_process_time;
    }

    switch (instruction->op) {
        struct semaphore *semaphore;
        int blocked;
    case READ:
    case WRITE:
    case PRINT:
        blocked = 0;
        syscall(PROCESS_INTERRUPT, &blocked);
        instruction->op = EXEC;
        instruction->value -= max_exec_time;
        process->remaining_time -= max_exec_time;
        kernel.cur_process_time += max_exec_time;
        break;
    case EXEC:
        if (instruction->value > max_exec_time) {
            instruction->value -= max_exec_time;
            process->remaining_time -= max_exec_time;
            kernel.cur_process_time += max_exec_time;
        } else {
            process->remaining_time -= instruction->value;
            kernel.cur_process_time += instruction->value;
            instruction->value = 0;
            process->pc++;
        }
        break;
    case P:
        semaphore = semaphore_find(&kernel.semaphore_table, instruction->sem);
        if (semaphore->handler_pid != (int)process->pid)
            syscall(SEMAPHORE_P, semaphore);

        if (process->status != BLOCKED) {
            instruction->op = EXEC;
            instruction->value -= max_exec_time;
            process->remaining_time -= max_exec_time;
            kernel.cur_process_time += max_exec_time;
        } else {
            process->pc++;
        }
        break;
    case V:
        semaphore = semaphore_find(&kernel.semaphore_table, instruction->sem);
        if (semaphore->handler_pid == (int)process->pid)
            syscall(SEMAPHORE_V, semaphore);
        instruction->op = EXEC;
        instruction->value -= max_exec_time;
        process->remaining_time -= max_exec_time;
        kernel.cur_process_time += max_exec_time;
        break;
    default:;
    }
}

/**
 * @brief Main kernel run function.
 */
void kernel_run()
{
    pdata_t *process = kernel.scheduler.atual;

    if (process == NULL || process->status == TERMINATED)
        return;

    if (kernel.cur_process_time >= process->quantum_time) {
        if (kernel.scheduler.ready_queue->size == 0) {
            kernel.cur_process_time = 0;
            return;
        }
        int is_blocked = 0;
        syscall(PROCESS_INTERRUPT, &is_blocked);
        return;
    }

    instruction_t *instruction = segment_fetch_instruction(
        &kernel.seg_table, process->seg_id, process->pc);

    exec_instruction(process, instruction, DEFAULT);

    if (process->remaining_time == 0 || process->pc == process->code_size) {
        syscall(PROCESS_FINISH, process);
    }
}

/**
 * @brief Shuts down the kernel.
 */
void kernel_shutdown()
{
    for (size_t i = 0; i < kernel.process_table.length; i++) {
        pdata_t *cur_proc = *(pdata_t **)vector_get(&kernel.process_table, i);
        program_destroy(cur_proc);
    }

    vector_destroy(&kernel.process_table);
    segment_table_destroy(&kernel.seg_table);
    // TODO: scheduler_destroy()
    // TODO: semaphore_table_destroy()
}

/**
 * @brief Gets a process by its PID.
 *
 * @param pid The process ID.
 * @return Pointer to the process data.
 */
pdata_t *kernel_get_process(size_t pid)
{
    if (kernel.process_table.length == 0)
        return NULL;

    return *(pdata_t **)vector_get(&kernel.process_table, pid - 1);
}

/**
 * @brief Gets the next available PID.
 *
 * @return The next PID.
 */
size_t get_next_pid()
{
    if (kernel.process_table.length == 0)
        return 1;

    pdata_t *last_proc = kernel_get_process(kernel.process_table.length);

    return last_proc->pid + 1;
}

/**
 * @brief Wakes up a process.
 *
 * @param pid The process ID.
 */
void wakeup(size_t pid) { sched_unlock_process(&(kernel.scheduler), pid); }
