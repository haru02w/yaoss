/**
 * @file memory.h
 * @brief Header file for memory management functions and data structures.
 */

#pragma once

#include "../process/instruction.h"
#include "../process/process.h"
#include "../util/vector.h"
#include <stdbool.h>

#define KILOBYTE (1024)
#define MEGABYTE (1024 * KILOBYTE)
#define GIGABYTE (1024 * MEGABYTE)

#define MAX_MEMORY_SIZE (1 * GIGABYTE)
#define PAGE_SIZE (8 * KILOBYTE)
#define INSTRUCTION_SIZE (2 * KILOBYTE)
#define MAX_PAGE_INSTRUCTION (PAGE_SIZE / INSTRUCTION_SIZE)

/**
 * @struct memory_request
 * @brief Represents a request to load a process into memory.
 */
struct memory_request {
    pdata_t *process;
    struct vector *instruction_list;
};

/**
 * @struct page
 * @brief Represents a page in memory.
 */
struct page {
    size_t id;
    instruction_t *code;
    size_t code_length;
    bool used_bit;
    bool on_disk;
};

/**
 * @struct segment
 * @brief Represents a segment in memory.
 */
struct segment {
    size_t id;
    size_t segment_size;
    struct page **page_table;
    struct page **resident_set;
    size_t page_table_size;
    size_t resident_set_size;
    size_t swap_page_id;
};

/**
 * @struct segment_table
 * @brief Represents the segment table in memory.
 */
struct segment_table {
    struct vector table;
    size_t remaining_memory;
};

/**
 * @brief Initializes the segment table.
 * @param seg_table Pointer to the segment table to initialize.
 */
void segment_table_init(struct segment_table *seg_table);

/**
 * @brief Searches for a segment in the segment table by its ID.
 * @param seg_table Pointer to the segment table.
 * @param seg_id ID of the segment to search for.
 * @return Index of the segment if found, 0 otherwise.
 */
size_t segment_table_search(struct segment_table *seg_table, size_t seg_id);

/**
 * @brief Removes a segment from the segment table by its ID.
 * @param seg_table Pointer to the segment table.
 * @param seg_id ID of the segment to remove.
 */
void segment_table_remove(struct segment_table *seg_table, size_t seg_id);

/**
 * @brief Destroys the segment table and frees all associated memory.
 * @param seg_table Pointer to the segment table.
 */
void segment_table_destroy(struct segment_table *seg_table);

/**
 * @brief Fetches an instruction from a segment by its program counter.
 * @param table Pointer to the segment table.
 * @param seg_id ID of the segment.
 * @param pc Program counter value.
 * @return Pointer to the instruction.
 */
instruction_t *segment_fetch_instruction(
    struct segment_table *table, size_t seg_id, size_t pc);

/**
 * @brief Handles a memory load request for a process.
 * @param seg_table Pointer to the segment table.
 * @param request Pointer to the memory request.
 * @return true if the memory load request was successful, false otherwise.
 */
bool mem_load_request(
    struct segment_table *seg_table, struct memory_request *request);
