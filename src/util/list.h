#ifndef UTIL_LIST_H_
#define UTIL_LIST_H_

#include <stddef.h>

typedef struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
} list_node_t;

typedef struct List {
    struct ListNode *head;
    struct ListNode *tail;
    int (*comparator)(void *, void *);
    size_t size;
} list_t;

void list_add(list_t *list, void *data);
void list_remove(list_t *list, void *data);
list_node_t *list_search(list_t *list, void *data);

#endif // UTIL_LIST_H_
