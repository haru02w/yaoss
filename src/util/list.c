#include "list.h"
#include <assert.h>
#include <stdlib.h>

list_t *list_create(int (*comparator)(const void *, const void *))
{
    list_t *new_list = malloc(sizeof *new_list);

    *new_list = (list_t) {
        .comparator = comparator, .head = NULL, .tail = NULL, .size = 0
    };

    return new_list;
}

list_node_t *list_node_create(void *data)
{
    list_node_t *new_node = malloc(sizeof *new_node);

    *new_node = (list_node_t) { .data = data, .next = NULL, .prev = NULL };

    return new_node;
}

void list_add(list_t *list, void *data)
{
    list_node_t *node = list_node_create(data);

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

void list_remove(list_t *list, void *key)
{
    assert(list != NULL && list->size > 0);

    list_node_t *found_node = list_search(list, key);

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

list_node_t *list_search(list_t *list, void *key)
{
    list_node_t *node = list->head;

    for (size_t i = 0; i < list->size; i++) {
        if (list->comparator(node->data, key) == 0) {
            return node;
        }
    }

    return NULL;
}

void list_free(list_t *list)
{
    assert(list != NULL);

    list_node_t *cur_node = list->head;
    list_node_t *tmp_node;

    for (size_t i = 0; i < list->size; i++) {
        tmp_node = cur_node->next;
        free(cur_node);
        cur_node = tmp_node;
    }

    free(list);
}
