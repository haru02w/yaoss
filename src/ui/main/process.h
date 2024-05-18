#pragma once
#include <curses.h>

struct ui_process {
    WINDOW *win;
    int highlight;
};

struct ui_process ui_create_process(WINDOW *parent_win);
void ui_render_process(struct ui_process *main_list);
void ui_destroy_process(struct ui_process *main_list);
