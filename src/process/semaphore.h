#pragma once
#include "../util/list.h"

struct semaphore {
    char name[10];
    unsigned int S;
    struct list *waiters;
};
