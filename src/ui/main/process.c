#include "process.h"
#include "../../core2ui.h"
#include "../../util/vector.h"
#include "../colors.h"
#include "curses.h"
#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
struct ui_process ui_process_create(WINDOW *parent_win)
{

    struct ui_process tmp = {
        .win = derwin(stdscr, (getmaxy(parent_win) - 1) * 3 / 5,
            getmaxx(parent_win) * 3 / 5, 0, 0),
        .menuwin
        = derwin(tmp.win, getmaxy(tmp.win) - 3, getmaxx(tmp.win) - 2, 2, 1),
        .highlight = 0,
        .proc_info = vector_create(sizeof(struct process_info)),
    };
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    box(tmp.win, 0, 0);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // paint white title line
    wmove(tmp.win, 1, 1);
    wattron(tmp.win, COLOR_PAIR(CP_TITLE));
    while (getcurx(tmp.win) < getmaxx(tmp.win) - 1)
        waddch(tmp.win, ' ');
    wattroff(tmp.win, COLOR_PAIR(CP_TITLE));

    char *title = "Process List";
    wmove(tmp.win, 0, (getmaxx(tmp.win) - strlen(title) - 2) / 2);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_RTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));
    waddstr(tmp.win, title);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_LTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    wattron(tmp.win, COLOR_PAIR(CP_TITLE));
    mvwprintw(tmp.win, 1, 1,
        "** "
        "%3.3s %12.12s %3.3s %c %9.9s %3.3s %6.6s %8.8s %5.5s %7.7s"
        " ",
        "PID", "NAME", "PRI", 'S', "PC/TOTAL", "SID", "MEM_KB", "TIME", "OP",
        "OPVAL");
    wattroff(tmp.win, COLOR_PAIR(CP_TITLE));
    wrefresh(tmp.win);
    return tmp;
}

void ui_process_render(
    struct ui_process *ui_process, struct sysio_info *sysio_info)
{

    /* NOTE: get data from core */

    werase(ui_process->menuwin);

    for (size_t i = 0; i < ui_process->proc_info.length; i++) {
        struct process_info *info = vector_get(&ui_process->proc_info, i);
        bool highlightit = ui_process->highlight == (int)i;

        if (info->process_id == sysio_info->running_process_id) {
            wattron(ui_process->menuwin, COLOR_PAIR(CP_LIST_ACTIVE));
        }
        if (ui_process->highlight == (int)i) {
            wattron(ui_process->menuwin, A_BOLD);
        }

        // Just to not fuck up the menu, i'm using MIN macro
        mvwprintw(ui_process->menuwin, i, 0,
            "%s "
            "%3.3hu %12.12s  %2.2hu %c %4.4lu/%-4.4lu %3.3hu %6.6lu %6.6luUT "
            "%5.5s %7.7s ",
            highlightit ? "->" : "  ", MIN(info->process_id, 999), info->name,
            MIN(info->priority, 99), info->process_state,
            MIN(info->program_counter, 9999), MIN(info->instr_total, 9999),
            MIN(info->segment_id, 999), MIN(info->memory_usage_kb, 999999),
            MIN(info->time_elapsed_ut, 999999), info->operation,
            info->operation_value);
        while (getcury(ui_process->menuwin) <(int)i + 1)
            waddch(ui_process->menuwin, ' ');
        wattroff(ui_process->menuwin, A_BOLD);
        wattroff(ui_process->menuwin, COLOR_PAIR(CP_LIST_ACTIVE));
    }
    wrefresh(ui_process->menuwin);
}

void ui_process_destroy(struct ui_process *ui_process)
{
    werase(ui_process->win);
    wrefresh(ui_process->win);
    vector_destroy(&ui_process->proc_info);
    delwin(ui_process->menuwin);
    delwin(ui_process->win);
}
