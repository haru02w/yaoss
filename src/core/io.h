#pragma once
#include "../process/process.h"
#include "../util/vector.h"

#define DISK_MAXIMUM_TRACK 200
#define DISK_MAXIMUM_SPEED 7200
#define DISK_MINIMUM_SPEED 7000

struct io_module {
    struct disk_module {
        struct vector request_list;
        unsigned int speed;
        unsigned int cur_track;
        char direction;

        unsigned int count_cycle;
        unsigned int cur_cycle;
        unsigned int rw_cycle;
        unsigned int move_cycle;

        size_t cur_request_id;
    } disk_module;
    struct vector print_queue;
};

enum io_disk_operation { DISK_READ, DISK_WRITE };

struct io_disk_request {
    size_t id;
    pdata_t *process;
    size_t track;
    enum io_disk_operation operation;
    unsigned int seek_time;
};

struct io_print_request {
    pdata_t *process;
    size_t remaining_time;
};

void io_module_init(struct io_module *io_module);

void io_module_destroy(struct io_module *io_module);

void io_disk_submit_request(struct disk_module *disk_module, pdata_t *process,
    size_t track, enum io_disk_operation operation);

void io_disk_schedule(struct disk_module *disk_module);

void io_print_submit_request(
    struct vector *print_queue, pdata_t *process, size_t print_time);

void io_print_schedule(struct vector *print_queue, size_t time);
