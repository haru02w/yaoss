#include "process.h"
#include "../../core2ui.h"
#include "../../util/vector.h"
#include "curses.h"

struct ui_process ui_create_process(WINDOW *parent_win)
{

    struct ui_process tmp = {
        .win = derwin(
            stdscr, getmaxy(parent_win) - 2, getmaxx(parent_win) / 2, 1, 0),
        .highlight = 0,
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
    struct vector *vec_process_info = get_processes_info(true);

    /* TODO: create menu items */
    for (int i = 0; i < (int)vec_process_info->length; i++) {
        struct process_info *info = vector_get(vec_process_info, i);
        if (ui_process->highlight == i)
            wattron(ui_process->win, A_REVERSE);
        mvwprintw(ui_process->win, i + 1, 1,
            "%3.3d %12.12s  %2.2d %4.4lu/%-4.4lu %3.3d %6.6fkUT",
            info->process_id, info->name, info->priority, info->program_counter,
            info->instr_total, info->segment_id, info->time_elapsed_ut);
        wattroff(ui_process->win, A_REVERSE);
    }
    wrefresh(ui_process->win);
}

void ui_destroy_process(struct ui_process *main_list)
{
    wborder(main_list->win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(main_list->win);
    delwin(main_list->win);
}
