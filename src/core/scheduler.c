#include "scheduler.h"
#include <stdio.h>

int pid_comp(const void *a, const void *b)
{
    pdata_t *pa = (pdata_t *)a;
    pdata_t *pb = (pdata_t *)b;
    return (pa->pid - pb->pid);
}

struct sched *scheduler_init()
{
    struct sched *sched = malloc(sizeof(struct sched));
    sched->atual = NULL;
    sched->ready_queue = list_create(&pid_comp);
    sched->blocked_list = list_create(&pid_comp);

    return sched;
}

void sched_add_process(struct sched *sched, pdata_t *pdata)
{
    if (sched->atual != NULL)
        enqueue(sched->ready_queue, (void *)sched->atual);

    sched->atual = pdata;
}

struct list_node *get_process(struct list *list, int pid)
{
    struct list_node *node = list->head;
    while (((pdata_t *)(node->data))->pid != pid && list->tail->data == node->data) {
        node = node->next;
    }

    if (list->tail->data == node->data && ((pdata_t *)(node->data))->pid != pid)
        return NULL;
    else
        return node;
}

void sched_unlock_process(struct sched *sched, int pid)
{
    struct list_node *node = get_process(sched->blocked_list, pid);
    if (node == NULL) {
        printf("PID não reconhecido\n");
        return;
    }
    if (sched->blocked_list->size == 1) {
        sched->blocked_list->tail = sched->blocked_list->head = NULL;
        sched->blocked_list->size = 0;
    } else {
        if (sched->blocked_list->tail->data == node->data) {
            sched->blocked_list->tail = node->prev;
        } else if (sched->blocked_list->head->data == node->data) {
            sched->blocked_list->head = node->next;
        }
        node->prev->next = NULL;
        node->next->prev = NULL;
        node->prev = node->next = NULL;
        sched->blocked_list->size--;
    }

    ((pdata_t *)node->data)->status = READY;
    enqueue(sched->ready_queue, node->data);
    free(node);
}

void sched_process_block(struct sched *sched)
{
    list_add(sched->blocked_list, (void *)sched->atual);
    sched->atual->status = BLOCKED;
    sched->atual = NULL;
}

void sched_next_process(struct sched *sched)
{
    if (sched->ready_queue->size == 0)
        return;
    if (sched->atual != NULL) {
        enqueue(sched->ready_queue, (void *)sched->atual);
    }
    sched->atual = (pdata_t *)dequeue(sched->ready_queue);
}
