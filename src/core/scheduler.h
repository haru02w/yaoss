#pragma once
#include "../process/process.h"
#include "../util/list.h"

struct sched {
    pdata_t *atual;
    struct list *ready_queue;
    struct list *blocked_list;
};
