#include "scheduler.h"

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
    sched->ready_list = list_create(&pid_comp);
    sched->blocked_list = list_create(&pid_comp);

    return sched;
}

void sched_add_process(struct sched *sched, pdata_t *pdata)
{
    struct list_node *node = sched->atual;
    struct list_node *new_node = list_node_create(pdata);

    if (node == NULL) {
        node = new_node;
        if (sched->ready_list->head == NULL) {
            sched->ready_list->head = sched->ready_list->tail = node;
            node->next = node->prev = node;
        } else {
            node->next = sched->ready_list->head;
            node->prev = sched->ready_list->tail;
            sched->ready_list->head = node;
        }
    } else {
        // TODO:process interrupt de node
        new_node->prev = node->prev;
        new_node->next = node;
        node->prev = new_node;
        if (!pid_comp((const void *)(sched->ready_list->head->data),
                (const void *)(node->data))) {
            sched->ready_list->tail = new_node;
        }
    }
    sched->ready_list->size = sched->ready_list->size + 1;
}
