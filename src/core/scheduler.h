#pragma once
#include "../process/process.h"
#include "../util/list.h"

struct sched {
    struct list_node *atual;
    struct list *ready_list;
    struct list *blocked_list;
};
