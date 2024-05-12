#pragma once
#include <curses.h>

struct ui_header {
    WINDOW *win;
};
struct ui_header ui_create_header(WINDOW *parent_win);
void ui_render_header(
    struct ui_header *main_header, double ut, uint64_t time_elapsed);
void ui_destroy_header(struct ui_header *main_header);
