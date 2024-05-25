#include "details.h"
#include "../../core2ui.h"
#include "curses.h"

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
    box(tmp.win, 0, 0);
    // box(tmp.menu_page, 0, 0);
    // box(tmp.menu_semaphore, 0, 0);

    // put a line in the middle
    mvwaddch(tmp.win, 0, getmaxx(tmp.win) / 2, ACS_TTEE);
    for (int i = 1; i < getmaxy(tmp.win) - 1; i++)
        mvwaddch(tmp.win, i, getmaxx(tmp.win) / 2, ACS_VLINE);
    mvwaddch(tmp.win, getmaxy(tmp.win) - 1, getmaxx(tmp.win) / 2, ACS_BTEE);

    // Semaphore table
    mvwprintw(tmp.win, 1, 1, "%4.4s %4.4s %4.4s", "NAME", "WPID", "WAIT");

    // Page table
    mvwprintw(tmp.win, 1, getmaxx(tmp.win) / 2 + 1, "%4.4s %5.5s %6.6s", "PGID",
        "USING", "ONDISK");

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
        // Just to not fuck up the menu, i'm using MIN macro
        mvwprintw(ui_details->menu_semaphore, i, 0, "%4.4s  %3.3hu %4.4lu",
            info->name, info->working_process_id, info->waiting_counter);
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
