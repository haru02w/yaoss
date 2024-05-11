#include "ui.h"
#include <curses.h>
#include <menu.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void footer(WINDOW *win);
void header(WINDOW *win);
void process_list(WINDOW *win);

bool paused = true;
uint64_t time_elapsed = 0;

void start_ui()
{
    /* Initialization */
    initscr(); // Start curses mode
    cbreak(); // disable line buffering except CTRL+C
    noecho(); // disable showing input text on screen
    keypad(stdscr, TRUE); // get F1-F12 and arrow keys as well
    nodelay(stdscr, TRUE); // don't block thread when getting input
    curs_set(0); // disable cursor

    int row = getmaxy(stdscr);
    int col = getmaxx(stdscr);

    WINDOW *win_header = derwin(stdscr, 1, col, 0, 0);
    WINDOW *win_process_list = derwin(stdscr, row - 2, col / 2, 1, 0);
    WINDOW *win_process_details
        = derwin(stdscr, row - 2, col / 2, 1, (col / 2));
    box(win_process_details, 0, 0);
    WINDOW *win_footer = derwin(stdscr, 1, col, row - 1, 0);

    footer(win_footer);
    process_list(win_process_list);
    wrefresh(win_process_list);

    double ut = 0.0001;
    while (true) {
        header(win_header);
        switch (getch()) {
        case KEY_UP:
            // TODO: menu_driver
            break;
        case KEY_DOWN:
            // TODO: menu_driver
            break;
        case ' ':
            paused = !paused;
            break;
        case 'c':
            werase(win_footer);
            mvwprintw(win_footer, 0, 0, " Path to synthetic program: ");
            char path[4096];
            echo();
            wgetstr(win_footer, path);
            noecho();
            // TODO: create_process(path)
            footer(win_footer);
            break;
        case 'q':
            goto end;
        }

        sleep(ut);
        time_elapsed += !paused;
    };
end:
    endwin();
}

void footer(WINDOW *win)
{
    werase(win);
    wprintw(win,
        " Q: stop simulation\tC: create process\tSPACE: pause simulation\t"
        "ENTER: show details ");
    wrefresh(win);
}

void header(WINDOW *win)
{
    // TODO: get memory usage
    // SYS_STATS
    int mem_usage = 999999999;
    int prev_usage = 0;

    // only redraw if needed
    if (mem_usage != prev_usage) {
        werase(win);
        mvwprintw(win, 0, 0, " memory usage: %d", mem_usage);
        prev_usage = mem_usage;
    }

    // CLOCK
    char s[128];
    int col = getmaxx(win);
    sprintf(s, "Time elapsed: %lukUT", time_elapsed / 1000);
    wmove(win, 0, (col - strlen(s)));
    wclrtoeol(win);
    mvwprintw(win, 0, (col - strlen(s)), "%s", s);
    wrefresh(win);
}

void process_list(WINDOW *win)
{
    int rows, cols;
    getmaxyx(win, rows, cols);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "%3.3s %12.12s %3.3s %9.9s %3.3s %9.9s", "PID", "NAME",
        "PRI", "PC", "SID", "TIME");

    // Creating strings
    char(*choices)[512];
    int qtd_rows = 3;
    choices = malloc(qtd_rows * sizeof(*choices));

    // TODO: get data not hardcoded
    sprintf(choices[0], "%3.3d %12.12s  %2.2d %4.4d/%-4.4d %3.3d %6.6dkUT", 0,
        "nome do programa", 20, 2, 5, 0, 234);
    sprintf(choices[1], "%3.3d %12.12s  %2.2d %4.4d/%-4.4d %3.3d %6.6dkUT", 1,
        "nome do programa2", 20, 3, 6, 1, 9);

    // Creating items
    ITEM **items = (ITEM **)calloc(qtd_rows + 1, sizeof(ITEM *));
    for (int i = 0; i < qtd_rows; ++i)
        items[i] = new_item(choices[i], NULL);

    MENU *menu = new_menu(items);
    set_menu_mark(menu, "");
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, rows - 2, cols - 2, 1, 1));
    post_menu(menu);
    wrefresh(win);
}
