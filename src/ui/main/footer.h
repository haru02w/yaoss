#pragma once

#include <curses.h>
struct ui_footer {
    WINDOW *win;
};
struct ui_footer ui_footer_create(WINDOW *parent_win);
char *ui_footer_ask_path(struct ui_footer *main_footer);
void ui_footer_render(struct ui_footer *main_footer);
void ui_footer_destroy(struct ui_footer *main_footer);
