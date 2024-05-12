#include "header.h"
#include "curses.h"
#include <stdint.h>
#include <string.h>
struct ui_header ui_create_header(WINDOW *parent_win)
{
    return (struct ui_header) {
        .win = derwin(parent_win, 1, getmaxx(parent_win), 0, 0),
    };
}

void ui_render_header(struct ui_header *main_header, double ut,
    uint64_t mem_usage, uint64_t time_elapsed)
{
    // TODO: get memory usage
    // SYS_STATS
    uint64_t prev_mem_usage = 0;

    // only redraw if needed
    if (mem_usage != prev_mem_usage) {
        werase(main_header->win);
        mvwprintw(main_header->win, 0, 0, " memory usage: %lu", mem_usage);
        prev_mem_usage = mem_usage;
    }

    // CLOCK
    char s0[128], s1[128];
    int col = getmaxx(main_header->win);
    sprintf(s0, "UT: %1.5lf", ut);
    sprintf(s1, "Time elapsed: %lukUT", time_elapsed / 1000);
    wmove(main_header->win, 0, (col - strlen(s0) - strlen(s1)));
    wclrtoeol(main_header->win);
    mvwprintw(main_header->win, 0, (col - strlen(s0) - strlen(s1) - 1), "%s %s",
        s0, s1);
    wrefresh(main_header->win);
}

void ui_destroy_header(struct ui_header *main_header)
{
    delwin(main_header->win);
}
