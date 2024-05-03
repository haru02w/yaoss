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
    int (*comparator)(const void *, const void *);
    size_t size;
} list_t;

list_t *list_create(int (*comparator)(const void *, const void *));
void list_add(list_t *list, void *data);
void list_remove(list_t *list, void *data);
list_node_t *list_search(list_t *list, void *data);
void list_free(list_t *list);

#endif // UTIL_LIST_H_
