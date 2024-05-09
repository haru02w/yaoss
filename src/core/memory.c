#include "memory.h"
#include <math.h>

void segment_table_init(struct segment_table *seg_table)
{
    seg_table->table = vector_create(sizeof(struct segment));
    seg_table->remaining_memory = MAX_MEMORY_SIZE;
}

static inline void segment_table_add(
    struct segment_table *seg_table, struct segment *seg)
{
    vector_push_back(&seg_table->table, seg);
}

static struct segment *segment_table_search(
    struct segment_table *seg_table, size_t seg_id)
{
    for (size_t i = 0; i < seg_table->table.length; i++) {
        struct segment *cur_seg
            = (struct segment *)vector_get(&seg_table->table, i);

        if (cur_seg->id == seg_id) {
            return cur_seg;
        }
    }

    return NULL;
}

static struct page *segment_page_search(struct segment *seg, size_t page_id)
{
    for (size_t i = 0; i < seg->resident_set_size; i++) {
        struct page *cur_page = (struct page *)vector_get(&seg->page_table, i);

        if (cur_page->id == page_id) {
            return cur_page;
        }
    }

    return NULL;
}

static struct page *segment_resident_search(struct segment *seg, size_t page_id)
{
    for (size_t i = 0; i < seg->resident_set_size; i++) {
        struct page *cur_page = seg->resident_set[i];

        if (cur_page->id == page_id) {
            return cur_page;
        }
    }

    return NULL;
}

void segment_table_remove(struct segment_table *seg_table, size_t seg_id)
{
    assert(seg_id < seg_table->table.length);

    struct segment *seg_found = segment_table_search(seg_table, seg_id);

    seg_table->remaining_memory += seg_found->segment_size;

    // vector_remove(&seg_table->table, seg_id);
}

void segment_table_destroy(struct segment_table *seg_table)
{
    for (size_t i = 0; i < seg_table->table.length; i++) {
        struct segment *cur_seg
            = (struct segment *)vector_get(&seg_table->table, i);

        for (size_t j = 0; j < cur_seg->page_table.length; j++) {
            struct page *cur_page
                = (struct page *)vector_get(&cur_seg->page_table, j);

            if (cur_page->code != NULL)
                free(cur_page->code);
        }

        vector_destroy(&cur_seg->page_table);
    }

    vector_destroy(&seg_table->table);
}

struct segment segment_create(pdata_t *process)
{
    struct segment new_segment;

    new_segment.id = process->seg;
    new_segment.page_table = vector_create(sizeof(struct page));
    new_segment.resident_set = NULL;
    new_segment.resident_set_size = 0;
    new_segment.segment_size = process->seg_size * KILOBYTE;
    new_segment.swap_page_id = 0;

    return new_segment;
}

static void segment_fill(struct segment *seg, struct vector *instruction_list)
{
    size_t total_instructions = instruction_list->length;
    size_t remaining_instructions = total_instructions;
    size_t max_page = ceil((double)seg->segment_size / PAGE_SIZE);
    size_t pages_needed
        = ceil((double)total_instructions / MAX_PAGE_INSTRUCTION);

    seg->resident_set_size = max_page;
    seg->resident_set = malloc(sizeof(struct page *) * seg->resident_set_size);

    for (size_t i = 0; i < pages_needed; i++) {
        size_t instructions_for_page
            = (remaining_instructions > MAX_PAGE_INSTRUCTION)
            ? MAX_PAGE_INSTRUCTION
            : remaining_instructions;

        struct page new_page;
        new_page.id = i;

        if (instructions_for_page > 0) {
            new_page.code
                = malloc(instructions_for_page * sizeof(instruction_t));
        }

        new_page.code_length = instructions_for_page;
        new_page.used_bit = 0;

        for (size_t j = 0; j < instructions_for_page; j++) {
            instruction_t *instruction = (instruction_t *)vector_get(
                instruction_list, total_instructions - remaining_instructions);
            new_page.code[j] = *instruction;
            remaining_instructions--;
        }

        vector_push_back(&seg->page_table, &new_page);

        if (i < seg->resident_set_size) {
            seg->resident_set[i]
                = (struct page *)vector_get(&seg->page_table, i);
        }
    }
}

static void mem_page_swap(struct segment *seg, struct page *new_page)
{
    struct page *cur_page = seg->resident_set[seg->swap_page_id];

    while (cur_page->used_bit != 0) {
        cur_page->used_bit = 0;
        seg->swap_page_id = (seg->swap_page_id + 1) % seg->resident_set_size;
        cur_page = seg->resident_set[seg->swap_page_id];
    }

    seg->resident_set[seg->swap_page_id] = new_page;
    seg->swap_page_id = (seg->swap_page_id + 1) % seg->resident_set_size;
}

opcode_t segment_fetch_instruction(
    struct segment_table *table, size_t seg_id, size_t pc)
{
    struct segment *found_segment = segment_table_search(table, seg_id);
    size_t page_id_nedeed = floor(pc / MAX_PAGE_INSTRUCTION);

    struct page *page_found
        = segment_resident_search(found_segment, page_id_nedeed);

    if (page_found == NULL) {
        page_found = segment_page_search(found_segment, page_id_nedeed);
        mem_page_swap(found_segment, page_found);
    }

    instruction_t found_instruction
        = page_found->code[pc % MAX_PAGE_INSTRUCTION];

    return found_instruction.op;
}

void mem_load_request(
    struct segment_table *seg_table, struct memory_request *request)
{
    struct segment seg = segment_create(request->process);

    seg_table->remaining_memory -= seg.segment_size;

    segment_fill(&seg, request->instruction_list);
    segment_table_add(seg_table, &seg);
}
