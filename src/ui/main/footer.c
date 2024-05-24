#include "footer.h"
#include <curses.h>

struct ui_footer ui_footer_create(WINDOW *parent_win)
{
    return (struct ui_footer) {
        .win
        = derwin(stdscr, 1, getmaxx(parent_win), getmaxy(parent_win) - 1, 0),
    };
}

char *ui_footer_ask_path(struct ui_footer *main_footer)
{
#define PATH_SIZE 4096
    static char path[PATH_SIZE];

    werase(main_footer->win);
    mvwprintw(main_footer->win, 0, 0, " Path to synthetic program: ");
    wgetnstr(main_footer->win, path, PATH_SIZE);
    return path;
}

void ui_footer_render(struct ui_footer *main_footer)
{
    werase(main_footer->win);
    wprintw(main_footer->win,
        " Q: close simulation    C: create process    SPACE: start/pause simulation ");
    wrefresh(main_footer->win);
}

void ui_footer_destroy(struct ui_footer *main_footer)
{
    delwin(main_footer->win);
}
