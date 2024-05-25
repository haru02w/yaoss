#pragma once

#include "../../util/vector.h"
#include "curses.h"

struct ui_details {
    WINDOW *win;
    WINDOW *menu_semaphore;
    WINDOW *menu_page;
    struct vector sem_info;
    struct vector page_info;
};

struct ui_details ui_details_create(WINDOW *parent_win);
void ui_details_render(struct ui_details *ui_details);
void ui_details_destroy(struct ui_details *ui_details);
