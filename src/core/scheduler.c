#include "scheduler.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/// @brief generic comparator for process id
/// @param a process void pointer a
/// @param b process void pointer a
/// @return  is 0 if pid is equal
///          is positive if a > b
///          is negative if a < b
int pid_comp(const void *a, const void *b)
{
    pdata_t *pa = (pdata_t *)a;
    pdata_t *pb = (pdata_t *)b;
    return (pa->pid - pb->pid);
}

/// @brief function to generate a new scheduler struct
/// @return scheduler pointer for the scheduler created
void scheduler_init(struct sched *sched)
{
    sched->atual = NULL;
    sched->ready_queue = list_create(&pid_comp);
    sched->blocked_list = list_create(&pid_comp);
}

/// @brief function that adds a new process to the scheduler
/// @param sched scheduler pointer
/// @param pdata program data to be added
void sched_add_process(struct sched *sched, pdata_t *pdata)
{
    if (sched->atual != NULL) {
        enqueue(sched->ready_queue, (void *)sched->atual);
        sched->atual->status = READY;
    }

    sched->atual = pdata;
    pdata->status = EXECUTING;
}

/// @brief function that searches for a process in the list or queue with a
///     given process id
/// @param list list where the search is performed
/// @param pid process id to be searched
/// @return
///     NULL if the process with this pid is not found
///     node pointer for the process with this pid
struct list_node *get_process(struct list *list, size_t pid)
{
    assert(list->size != 0);
    struct list_node *node = list->head;

    while (node != NULL) {
        if (((pdata_t *)(node->data))->pid == pid) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/// @brief function to unlock a blocked process
/// @param sched scheduler pointer
/// @param pid process id number of the process to be unlocked
void sched_unlock_process(struct sched *sched, size_t pid)
{
    struct list_node *node = get_process(sched->blocked_list, pid);
    if (node == NULL) {
        printf("PID não reconhecido\n");
        return;
    }

    if (sched->blocked_list->size == 1) {
        sched->blocked_list->head = sched->blocked_list->tail = NULL;
    } else {
        if (node == sched->blocked_list->head) {
            sched->blocked_list->head = node->next;
            if (sched->blocked_list->head != NULL) {
                sched->blocked_list->head->prev = NULL;
            }
        } else if (node == sched->blocked_list->tail) {
            sched->blocked_list->tail = node->prev;
            if (sched->blocked_list->tail != NULL) {
                sched->blocked_list->tail->next = NULL;
            }
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }

    sched->blocked_list->size--;

    ((pdata_t *)node->data)->status = READY;
    enqueue(sched->ready_queue, node->data);
    free(node);
}

/// @brief function that blocks the currently running process
/// @param sched scheduler pointer
void sched_process_block(struct sched *sched)
{
    if (sched->atual == NULL) {
        printf("erro não há processo escalonado");
        return;
    }
    list_add(sched->blocked_list, (void *)sched->atual);
    sched->atual->status = BLOCKED;
    sched->atual = NULL;
}

/// @brief function that runs the first process in the ready queue
///     (if a process is already running, it is moved to the end of the queue)
/// @param sched scheduler pointer
void sched_next_process(struct sched *sched)
{
    if (sched->ready_queue->size == 0)
        return;
    if (sched->atual != NULL) {
        sched->atual->status = READY;
        enqueue(sched->ready_queue, (void *)sched->atual);
    }
    sched->atual = (pdata_t *)dequeue(sched->ready_queue);
    sched->atual->status = EXECUTING;
}

/// @brief function that remove the process that reach at end
/// @param sched scheduler pointer
void sched_remove(struct sched *sched)
{
    if (sched->atual == NULL) {
        printf("erro não há processo escalonado");
        return;
    }
    sched->atual->status = TERMINATED;
    sched->atual = NULL;
}

void scheduler_destroy(struct sched *sched)
{
    list_destroy(sched->ready_queue);
    list_destroy(sched->blocked_list);
    sched->atual = NULL;
}
