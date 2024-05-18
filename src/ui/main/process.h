#pragma once
#include "../../util/vector.h"
#include <curses.h>

struct ui_process {
    WINDOW *win;
    WINDOW *menuwin;
    int highlight;
    struct vector *proc_info;
};

struct ui_process ui_create_process(WINDOW *parent_win);
void ui_render_process(struct ui_process *main_list);
void ui_destroy_process(struct ui_process *main_list);
