#pragma once
#include "../../util/vector.h"
#include <curses.h>

struct ui_process {
    WINDOW *win;
    WINDOW *menuwin;
    int highlight;
    struct vector *proc_info;
};

struct ui_process ui_process_create(WINDOW *parent_win);
void ui_process_render(struct ui_process *ui_process);
void ui_process_destroy(struct ui_process *ui_process);
