#pragma once
#include "../process/process.h"
#include "../util/list.h"

/// @brief Struct for the scheduler
///     - atual: currently running process
///     - ready_queue: queue for ready processes
///     - blocked_list: list that contains blocked processes
struct sched {
    pdata_t *atual;
    struct list *ready_queue;
    struct list *blocked_list;
};

/// @brief function to generate a new scheduler struct
/// @return scheduler pointer for the scheduler created
struct sched *scheduler_init();

/// @brief function that adds a new process to the scheduler
/// @param sched scheduler pointer
/// @param pdata program data to be added
void sched_add_process(struct sched *sched, pdata_t *pdata);

/// @brief function that searches for a process in the list or queue with a
///     given process id
/// @param list list where the search is performed
/// @param pid process id to be searched
/// @return
///     NULL if the process with this pid is not found
///     node pointer for the process with this pid
struct list_node *get_process(struct list *list, size_t pid);

/// @brief function to unlock a blocked process
/// @param sched scheduler pointer
/// @param pid process id number of the process to be unlocked
void sched_unlock_process(struct sched *sched, size_t pid);

/// @brief function that blocks the currently running process
/// @param sched scheduler pointer
void sched_process_block(struct sched *sched);

/// @brief function that runs the first process in the ready queue
///     (if a process is already running, it is moved to the end of the queue)
/// @param sched scheduler pointer
void sched_next_process(struct sched *sched);
