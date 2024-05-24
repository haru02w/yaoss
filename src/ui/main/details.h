#pragma once

#include "curses.h"
struct ui_details {
    WINDOW *win;
    WINDOW *menu_page;
    WINDOW *menu_semaphore;
};

struct ui_details ui_details_create(WINDOW *parent_win);
void ui_details_render(struct ui_details *ui_details);
void ui_details_destroy(struct ui_details *ui_details);
