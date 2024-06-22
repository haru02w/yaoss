// TODO:
#pragma once
#include "../../util/vector.h"
#include "curses.h"

struct ui_io {
    WINDOW *win;
    WINDOW *menuwin;
    int highlight;
    struct vector io_info;
};

struct ui_io ui_io_create(WINDOW *parent_win);
void ui_io_render(struct ui_io *ui_io);
void ui_io_destroy(struct ui_io *ui_io);
