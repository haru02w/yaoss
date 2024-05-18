#include "process.h"
#include "../../core2ui.h"
#include "../../util/vector.h"
#include "curses.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
struct ui_process ui_create_process(WINDOW *parent_win)
{

    struct ui_process tmp = {
        .win = derwin(
            stdscr, getmaxy(parent_win) - 2, getmaxx(parent_win) / 2, 1, 0),
        .menuwin
        = derwin(tmp.win, getmaxy(tmp.win) - 3, getmaxx(tmp.win) - 2, 2, 1),
        .highlight = 0,
        .proc_info = NULL,
    };
    box(tmp.win, 0, 0);
    mvwprintw(tmp.win, 1, 1, "%3.3s %12.12s %3.3s %9.9s %3.3s %9.9s", "PID",
        "NAME", "PRI", "PC", "SID", "TIME");
    wrefresh(tmp.win);
    return tmp;
}

void ui_render_process(struct ui_process *ui_process)
{

    /* get data from core */
    werase(ui_process->menuwin);

    for (size_t i = 0; i < ui_process->proc_info->length; i++) {
        struct process_info *info = vector_get(ui_process->proc_info, i);
        if (ui_process->highlight == (int)i)
            wattron(ui_process->menuwin, A_REVERSE);

        // Just to not fuck up the menu, i'm using MIN macro
        mvwprintw(ui_process->menuwin, i, 0,
            "%3.3hu %12.12s  %2.2hu %4.4lu/%-4.4lu %3.3hu %6.6lukUT",
            MIN(info->process_id, 999), info->name, MIN(info->priority, 99),
            MIN(info->program_counter, 9999), MIN(info->instr_total, 9999),
            MIN(info->segment_id, 999), MIN(info->time_elapsed_ut, 999999));
        wattroff(ui_process->menuwin, A_REVERSE);
    }
    wrefresh(ui_process->menuwin);
}

void ui_destroy_process(struct ui_process *ui_process)
{
    werase(ui_process->win);
    wrefresh(ui_process->win);
    vector_destroy(ui_process->proc_info);
    delwin(ui_process->menuwin);
    delwin(ui_process->win);
}
