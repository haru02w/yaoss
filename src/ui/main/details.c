#include "details.h"
#include "../../core2ui.h"
#include "../colors.h"
#include "curses.h"
#include <string.h>

static const char *truestr = "true";
static const char *falsestr = "false";

struct ui_details ui_details_create(WINDOW *parent_win)
{
    int row, col;
    getmaxyx(parent_win, row, col);
    struct ui_details tmp = {
        .win = derwin(stdscr, row - 2, col * 2 / 5, 1, col * 3 / 5),
        .menu_semaphore
        = derwin(tmp.win, getmaxy(tmp.win) - 3, getmaxx(tmp.win) / 2 - 1, 2, 1),
        .menu_page = derwin(tmp.win, getmaxy(tmp.win) - 3,
            getmaxx(tmp.win) / 2 - 2, 2, getmaxx(tmp.win) / 2 + 1),
        .page_info = vector_create(sizeof(struct page_info)),
        .sem_info = vector_create(sizeof(struct semaphore_info)),
    };
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    box(tmp.win, 0, 0);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // Write semaphore title
    char *title_sem = "Semaphore List";
    wmove(tmp.win, 0, ((getmaxx(tmp.win) / 2) - strlen(title_sem) - 2) / 2);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_RTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));
    waddstr(tmp.win, title_sem);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_LTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // Write pages title
    char *title_page = "Pages List";
    wmove(tmp.win, 0,
        (getmaxx(tmp.win) / 2)
            + ((getmaxx(tmp.win) / 2) - strlen(title_sem) - 2) / 2);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_RTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));
    waddstr(tmp.win, title_page);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_LTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // put a line in the middle
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    mvwaddch(tmp.win, 0, getmaxx(tmp.win) / 2, ACS_TTEE);
    for (int i = 1; i < getmaxy(tmp.win) - 1; i++)
        mvwaddch(tmp.win, i, getmaxx(tmp.win) / 2, ACS_VLINE);
    mvwaddch(tmp.win, getmaxy(tmp.win) - 1, getmaxx(tmp.win) / 2, ACS_BTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // Semaphore table
    wattron(tmp.win,COLOR_PAIR(CP_TITLE));
    mvwprintw(tmp.win, 1, 1, "%4.4s %4.4s %4.4s", "NAME", "WPID", "WAIT");
    wattroff(tmp.win,COLOR_PAIR(CP_TITLE));

    // Page table
    wattron(tmp.win,COLOR_PAIR(CP_TITLE));
    mvwprintw(tmp.win, 1, getmaxx(tmp.win) / 2 + 1, "%4.4s %5.5s %6.6s", "PGID",
        "USING", "ONDISK");
    wattroff(tmp.win,COLOR_PAIR(CP_TITLE));

    wrefresh(tmp.win);
    return tmp;
}

void ui_details_render(struct ui_details *ui_details)
{
    werase(ui_details->menu_semaphore);
    werase(ui_details->menu_page);

    // semaphore table
    for (size_t i = 0; i < ui_details->sem_info.length; ++i) {
        struct semaphore_info *info = vector_get(&ui_details->sem_info, i);

        // handle semaphore with no use
        char wpid[4];
        sprintf(wpid, info->working_process_id < 0 ? "   " : "%3.3hu",
            (uint16_t)info->working_process_id);

        mvwprintw(ui_details->menu_semaphore, i, 0, "%4.4s  %3.3s %4.4lu",
            info->name, wpid, info->waiting_counter);
    }

    // page table
    for (size_t i = 0; i < ui_details->page_info.length; ++i) {
        struct page_info *info = vector_get(&ui_details->page_info, i);
        mvwprintw(ui_details->menu_page, i, 0, "%4.4d %-5.5s  %-5.5s",
            info->page_id, info->using ? truestr : falsestr,
            info->on_disk ? truestr : falsestr);
    }

    wrefresh(ui_details->menu_semaphore);
    wrefresh(ui_details->menu_page);
}

void ui_details_destroy(struct ui_details *ui_details)
{
    vector_destroy(&ui_details->sem_info);
    vector_destroy(&ui_details->page_info);
    werase(ui_details->win);
    delwin(ui_details->win);
    delwin(ui_details->menu_page);
    delwin(ui_details->menu_semaphore);
}
