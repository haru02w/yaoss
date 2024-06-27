#include "io.h"
#include "kernel.h"
#include <stdlib.h>

void io_module_init(struct io_module *io_module)
{
    unsigned int random_speed = DISK_MINIMUM_SPEED
        + rand() % (DISK_MAXIMUM_SPEED - DISK_MINIMUM_SPEED);

    *io_module = (struct io_module) { .disk_module
        = { .request_list = vector_create(sizeof(struct io_disk_request)),
            .speed = random_speed,
            .cur_track = 0,
            .direction = 'R',
            .count_cycle = 0,
            .move_cycle = 200 * (DISK_MAXIMUM_TRACK / (float)random_speed),
            .rw_cycle = 20000 * (DISK_MAXIMUM_TRACK / (float)random_speed),
            .cur_request_id = -1 },
        .print_queue = vector_create(sizeof(struct io_print_request)) };

    io_module->disk_module.cur_cycle = io_module->disk_module.move_cycle;
}

static int io_disk_request_cmp(const void *a, const void *b)
{
    struct io_disk_request *request_1 = (struct io_disk_request *)a;
    struct io_disk_request *request_2 = (struct io_disk_request *)b;
    return request_1->track - request_2->track;
}

static size_t io_disk_get_next_id(struct vector *request_list)
{
    size_t max_id = 0;

    for (size_t i = 0; i < request_list->length; i++) {
        struct io_disk_request *req = vector_get(request_list, i);
        if (req->id > max_id)
            max_id = req->id;
    }

    return max_id + 1;
}

void io_disk_submit_request(struct disk_module *disk_module, pdata_t *process,
    size_t track, enum io_disk_operation operation)
{
    struct io_disk_request request
        = { .id = io_disk_get_next_id(&disk_module->request_list),
              .process = process,
              .track = track,
              .operation = operation };

    if (disk_module->cur_track > track) {
        if (disk_module->direction == 'R') {
            request.seek_time = (DISK_MAXIMUM_TRACK - disk_module->cur_track)
                + (DISK_MAXIMUM_TRACK - track);
        } else {
            request.seek_time = disk_module->cur_track - track;
        }
    } else {
        if (disk_module->direction == 'L') {
            request.seek_time = disk_module->cur_track + track;
        } else {
            request.seek_time = track - disk_module->cur_track;
        }
    }

    vector_push_back(&disk_module->request_list, &request);
    vector_sort(&disk_module->request_list, io_disk_request_cmp);
}

static inline struct io_disk_request *io_disk_visit_track(
    struct vector *request_list, size_t track)
{
    struct io_disk_request key = { .track = track };
    return bsearch(&key, request_list->data, request_list->length,
        sizeof(struct io_disk_request), io_disk_request_cmp);
}

static void io_disk_resolve_request(struct disk_module *disk_module)
{
    struct io_disk_request *request = io_disk_visit_track(
        &disk_module->request_list, disk_module->cur_track);
    size_t request_id
        = request - ((struct io_disk_request *)disk_module->request_list.data);
    syscall(DISK_FINISH, request->process);
    vector_remove(&disk_module->request_list, request_id);
}

static void io_disk_update_direction(struct disk_module *disk_module)
{
    if (disk_module->direction == 'R') {
        if (disk_module->cur_track >= DISK_MAXIMUM_TRACK) {
            disk_module->direction = 'L';
        } else {
            disk_module->cur_track++;
        }
    } else {
        if (disk_module->cur_track <= 0) {
            disk_module->direction = 'R';
        } else {
            disk_module->cur_track--;
        }
    }
}

void io_disk_schedule(struct disk_module *disk_module)
{
    if (disk_module->count_cycle < disk_module->cur_cycle) {
        disk_module->count_cycle++;
        return;
    }

    disk_module->count_cycle = 0;

    disk_module->speed = DISK_MINIMUM_SPEED
        + rand() % (DISK_MAXIMUM_SPEED - DISK_MINIMUM_SPEED);

    if (disk_module->cur_cycle == disk_module->move_cycle) {
        struct io_disk_request *req = io_disk_visit_track(
            &disk_module->request_list, disk_module->cur_track);
        if (req != NULL) {
            disk_module->cur_cycle = disk_module->rw_cycle;
            disk_module->cur_request_id = req->id;
            return;
        }
    } else {
        io_disk_resolve_request(disk_module);
        struct io_disk_request *req = io_disk_visit_track(
            &disk_module->request_list, disk_module->cur_track);
        if (req != NULL) {
            disk_module->cur_request_id = req->id;
            return;
        }
        disk_module->cur_cycle = disk_module->move_cycle;
        disk_module->cur_request_id = -1;
    }

    io_disk_update_direction(disk_module);
}

void io_print_submit_request(
    struct vector *print_queue, pdata_t *process, size_t print_time)
{
    struct io_print_request request
        = { .process = process, .remaining_time = print_time };
    vector_push_back(print_queue, &request);
}

void io_print_schedule(struct vector *print_queue, size_t time)
{
    if (print_queue->length < 1)
        return;

    struct io_print_request *request = vector_get(print_queue, 0);
    if (request->remaining_time - time <= 0) {
        syscall(PRINT_FINISH, request->process);
        vector_remove(print_queue, 0);
    } else {
        request->remaining_time -= time;
    }
}

void io_module_destroy(struct io_module *io_module)
{
    vector_destroy(&io_module->disk_module.request_list);
    vector_destroy(&io_module->print_queue);
}
