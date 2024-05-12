#include "details.h"
#include "curses.h"
struct ui_details ui_create_details(WINDOW *parent_win)
{
    int row, col;
    getmaxyx(parent_win, row, col);
    struct ui_details tmp = {
        .win = derwin(stdscr, row - 2, col / 2, 1, (col / 2)),
    };
    box(tmp.win, 0, 0);
    return tmp;
}

void ui_destroy_details(struct ui_details *ui_details)
{
    wborder(ui_details->win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    delwin(ui_details->win);
}
