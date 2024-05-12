#pragma once

#include "curses.h"
struct ui_details {
    WINDOW *win;
};

struct ui_details ui_create_details(WINDOW *parent_win);
void ui_destroy_details(struct ui_details *ui_details);
