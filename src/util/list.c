#include "list.h"
#include <assert.h>
#include <stdlib.h>

struct list *list_create(int (*comparator)(const void *, const void *))
{
    struct list *new_list = (struct list *)malloc(sizeof *new_list);

    *new_list = (struct list) {
        .comparator = comparator, .head = NULL, .tail = NULL, .size = 0
    };

    return new_list;
}

struct list_node *list_node_create(void *data)
{
    struct list_node *new_node = (struct list_node *)malloc(sizeof *new_node);

    *new_node = (struct list_node) {
        .data = (byte *)data, .next = NULL, .prev = NULL
    };

    return new_node;
}

void list_add(struct list *list, void *data)
{
    struct list_node *node = list_node_create(data);

    assert(node != NULL);

    if (list->head == NULL)
        list->head = node;
    else {
        list->tail->next = node;
        node->prev = list->tail;
    }

    list->tail = node;
    list->size++;
}

void list_remove(struct list *list, void *key)
{
    assert(list != NULL && list->size > 0);

    struct list_node *found_node = list_search(list, key);

    if (found_node == NULL) {
        return;
    }

    if (found_node == list->head) {
        list->head = found_node->next;

        if (list->head != NULL) {
            list->head->prev = NULL;
        }
    } else {
        found_node->prev->next = found_node->next;

        if (found_node->next != NULL) {
            found_node->next->prev = found_node->prev;
        }
    }

    free(found_node);
    list->size--;
}

struct list_node *list_search(struct list *list, void *key)
{
    struct list_node *node = list->head;

    for (size_t i = 0; i < list->size; i++) {
        if (list->comparator(node->data, key) == 0) {
            return node;
        }
    }

    return NULL;
}

void list_destroy(struct list *list)
{
    assert(list != NULL);

    struct list_node *cur_node = list->head;
    struct list_node *tmp_node;

    for (size_t i = 0; i < list->size; i++) {
        tmp_node = cur_node->next;
        free(cur_node);
        cur_node = tmp_node;
    }

    free(list);
}

void *dequeue(struct list *queue)
{
    struct list_node *node = queue->head;
    if (node == NULL)
        return NULL;
    queue->head = node->next;
    queue->head->prev = NULL;
    queue->size--;
    return node->data;
}

void enqueue(struct list *queue, void *data)
{
    struct list_node *node = list_node_create(data);
    if (queue->size == 0) {
        queue->head = queue->tail = node;
        queue->size = 1;
        return;
    }
    queue->tail->next = node;
    node->prev = queue->tail;
    queue->tail = node;
    node->next = NULL;
    queue->size++;
}
