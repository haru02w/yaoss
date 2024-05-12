#pragma once
#include <curses.h>
#include <menu.h>

struct ui_process {
    WINDOW *win;
    MENU *menu;
    ITEM **items;
    int items_length;
    char (*item_str)[512];
};

struct ui_process ui_create_process(WINDOW *parent_win);
void ui_render_process(struct ui_process *main_list);
void ui_destroy_process(struct ui_process *main_list);
