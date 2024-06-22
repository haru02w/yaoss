// TODO:
#include "io.h"
#include "curses.h"

struct ui_io ui_io_create(WINDOW *parent_win)
{
    struct ui_io tmp = {
        .win = derwin(stdscr, (getmaxy(parent_win) - 2) * 2 / 5,
            getmaxx(parent_win) * 3 / 5, (getmaxy(parent_win) - 2) * 3 / 5, 0),
        .menuwin
        = derwin(tmp.win, getmaxy(tmp.win) - 3, getmaxx(tmp.win) - 2, 2, 1),
        .highlight = 0,
        /* .io_info = vector_create(sizeof(struct io_info)), */
    };
    return tmp;
};

void ui_io_render(struct ui_io *ui_io) {
}
void ui_io_destroy(struct ui_io *ui_io) { }
