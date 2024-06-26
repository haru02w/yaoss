#pragma once
#include "byte.h"
#include <stddef.h>

struct list_node {
    byte *data;
    struct list_node *next;
    struct list_node *prev;
};

struct list {
    struct list_node *head;
    struct list_node *tail;
    int (*comparator)(const void *, const void *);
    size_t size;
};

struct list *list_create(int (*comparator)(const void *, const void *));
void list_add(struct list *list, void *data);
void list_remove(struct list *list, void *data);
struct list_node *list_search(struct list *list, void *data);
void list_destroy(struct list *list);
struct list_node *list_node_create(void *data);
void *dequeue(struct list *queue);
void enqueue(struct list *queue, void *data);
