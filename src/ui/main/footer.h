#pragma once

#include <curses.h>
struct ui_footer {
    WINDOW *win;
};
struct ui_footer ui_create_footer(WINDOW *parent_win);
char *ui_ask_path_footer(struct ui_footer *main_footer);
void ui_render_footer(struct ui_footer *main_footer);
void ui_destroy_footer(struct ui_footer *main_footer);
