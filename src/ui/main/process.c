#include "process.h"
#include "../../core2ui.h"
#include "../../util/vector.h"
#include "curses.h"
#include "menu.h"
#include <assert.h>
#include <stdlib.h>

static char *header_format = "%3.3s %12.12s %3.3s %9.9s %3.3s %9.9s";
static char *item_format = "%3.3d %12.12s  %2.2d %4.4d/%-4.4d %3.3d %6.6dkUT";

struct ui_process ui_create_process(WINDOW *parent_win)
{

    struct ui_process tmp = {
        .win = derwin(
            stdscr, getmaxy(parent_win) - 2, getmaxx(parent_win) / 2, 1, 0),
        .items = NULL,
        .item_str = NULL,
    };
    box(tmp.win, 0, 0);
    mvwprintw(tmp.win, 1, 1, header_format, "PID", "NAME", "PRI", "PC", "SID",
        "TIME");
    tmp.menu = new_menu(NULL);
    set_menu_mark(tmp.menu, "");
    set_menu_win(tmp.menu, tmp.win);
    set_menu_sub(tmp.menu,
        derwin(tmp.win, getmaxy(tmp.win) - 3, getmaxx(tmp.win) - 2, 2, 1));
    post_menu(tmp.menu);
    wrefresh(tmp.win);
    return tmp;
}

void ui_render_process(struct ui_process *ui_process)
{ // TODO: my brain can't handle it no more, i'm sorry
    void *ptr; // tmp pointer

    /* get data from core */
    struct vector *vec_process_info = get_processes_info(true);

    /* create strings from menu */
    ptr = realloc(ui_process->item_str,
        vec_process_info->length * sizeof(*ui_process->item_str));
    assert(ptr != NULL);
    ui_process->item_str = ptr;

    /* fill strings with useful data */
    for (size_t i = 0; i < vec_process_info->length; i++) {
        struct process_info *info = vector_get(vec_process_info, i);
        sprintf(ui_process->item_str[i], item_format, info->process_id,
            info->name, info->priority, info->program_counter,
            info->instr_total, info->segment_id, info->time_elapsed_ut);
    }

    /* create menu items */

#if 0
    // Creating items
    ui_process->items
        = (ITEM **)calloc(ui_process->items_length + 1, sizeof(ITEM *));
    for (int i = 0; i < ui_process->items_length; ++i)
        ui_process->items[i] = new_item(ui_process->item_str[i], NULL);

    ui_process->menu = new_menu(ui_process->items);

    set_menu_mark(ui_process->menu, "");
    set_menu_win(ui_process->menu, ui_process->win);
    set_menu_sub(ui_process->menu,
        derwin(ui_process->win, getmaxy(ui_process->win) - 2,
            getmaxx(ui_process->win) - 2, 1, 1));
    post_menu(ui_process->menu);
#endif
    wrefresh(ui_process->win);
}

void ui_destroy_process(struct ui_process *main_list)
{
    unpost_menu(main_list->menu);
    free_menu(main_list->menu);
    for (int i = 0; i < main_list->items_length; ++i)
        free_item(main_list->items[i]);
    free(main_list->items);
    free(main_list->item_str);
    wborder(main_list->win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(main_list->win);
    delwin(main_list->win);
}
