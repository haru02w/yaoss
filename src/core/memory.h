#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "../process/instruction.h"
#include "../process/process.h"
#include "../util/vector.h"

#define KILOBYTE (1024)
#define MEGABYTE (1024 * KILOBYTE)
#define GIGABYTE (1024 * MEGABYTE)

#define MAX_MEMORY_SIZE (1 * GIGABYTE)
#define PAGE_SIZE (8 * KILOBYTE)
#define INSTRUCTION_SIZE (0.5 * KILOBYTE)
#define MAX_PAGE_INSTRUCTION (PAGE_SIZE / INSTRUCTION_SIZE)

struct memory_request {
    pdata_t *process;
    struct vector *instruction_list;
};

struct page {
    size_t id;
    instruction_t *code;
    size_t code_length;
    int used_bit;
};

struct segment {
    size_t id;
    struct vector page_table;
    size_t segment_size;
};

struct segment_table {
    struct vector table;
    size_t remaining_memory;
};

void segment_table_init(struct segment_table *seg_table);
void segment_table_destroy(struct segment_table *seg_table);
void mem_load_request(
    struct segment_table *seg_table, struct memory_request *request);
