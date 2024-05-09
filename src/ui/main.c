#include "main.h"
#include "curses.h"
#include <ncurses.h>
#include <stdlib.h>

void *ui_main()
{
    // TODO: ncurses
    initscr();
    printw("Hello World");
    refresh();
    getch();
    endwin();

    return NULL;
}
