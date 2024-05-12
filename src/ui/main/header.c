#include "header.h"
#include "../../core2ui.h"
#include "curses.h"
#include <stdint.h>
#include <string.h>
struct ui_header ui_create_header(WINDOW *parent_win)
{
    return (struct ui_header) {
        .win = derwin(parent_win, 1, getmaxx(parent_win), 0, 0),
    };
}

void ui_render_header(
    struct ui_header *main_header, double ut, uint64_t time_elapsed)
{
    static struct simulation_info prev_info = { 0 };

    struct simulation_info info = get_simulation_info();

    // only redraw if needed
    if (info.memory_usage_mb != prev_info.memory_usage_mb) {
        werase(main_header->win);
        mvwprintw(main_header->win, 0, 0, " memory usage: %lu/1024MB",
            info.memory_usage_mb);
        prev_info.memory_usage_mb = info.memory_usage_mb;
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
