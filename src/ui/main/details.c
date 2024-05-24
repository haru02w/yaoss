#include "details.h"
#include "curses.h"

struct ui_details ui_details_create(WINDOW *parent_win)
{
    int row, col;
    getmaxyx(parent_win, row, col);
    struct ui_details tmp = {
        .win = derwin(stdscr, row - 2, col / 2, 1, (col / 2)),
        .menu_page = derwin(
            tmp.win, (getmaxy(tmp.win) / 2) - 1, getmaxx(tmp.win) - 2, 1, 1),
        .menu_semaphore = derwin(tmp.win, (getmaxy(tmp.win) / 2) - 1,
            getmaxx(tmp.win) - 2, getmaxy(tmp.win) / 2 + 1, 1),
    };
    box(tmp.win, 0, 0);

    // put a line in the middle
    mvwaddch(tmp.win, getmaxy(tmp.win) / 2, 0, ACS_LTEE);
    for (int i = 0; i < getmaxx(tmp.win) - 2; i++)
        waddch(tmp.win, ACS_HLINE);
    waddch(tmp.win, ACS_RTEE);

    wrefresh(tmp.win);
    return tmp;
}

void ui_details_render(struct ui_details *ui_details)
{
    mvwprintw(ui_details->win, 0, 0, "%3.3s %c %c", "ID", 'U', 'D');
    // TODO: page table
    // TODO: semaphore table
}

void ui_details_destroy(struct ui_details *ui_details)
{
    werase(ui_details->win);
    delwin(ui_details->win);
    delwin(ui_details->menu_page);
    delwin(ui_details->menu_semaphore);
}
