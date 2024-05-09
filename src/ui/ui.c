#include "ui.h"
#include "curses.h"
#include <ncurses.h>
#include <string.h>
#include <time.h>

static bool paused = true;
void footer(WINDOW *win);
void header(WINDOW *win);
void content(WINDOW *win);

double time_elapsed()
{
    static clock_t time = 0;
    static clock_t time_prev = 0;

    if (!time) {
        time = time_prev = clock();
        return 0.0;
    }

    clock_t now = clock();
    if (paused)
        time += now - time_prev;

    time_prev = now;
    return (((double)(now - time)) / CLOCKS_PER_SEC);
}

void start_ui()
{
    /*
     * Initialization:
     */
    initscr(); // Start curses mode
    cbreak(); // disable line buffering except CTRL+C
    noecho(); // disable showing input text on screen
    keypad(stdscr, TRUE); // get F1-F12 and arrow keys as well
    nodelay(stdscr, TRUE); // don't block thread when getting input

    int row = getmaxy(stdscr);
    int col = getmaxx(stdscr);

    WINDOW *win_header = derwin(stdscr, 1, col, 0, 0);
    WINDOW *win_content = derwin(stdscr, row - 2, col, 1, 0);
    WINDOW *win_footer = derwin(stdscr, 1, col, row - 1, 0);

    footer(win_footer);
    content(win_content);
    wrefresh(win_content);
    while (true) {
        header(win_header);
        switch (getch()) {
        case 'q':
            goto end;
        case KEY_ENTER:
            // TODO: show details
        case ' ':
            paused = !paused;
            break;
        }
    };
end:
    endwin();
}

void footer(WINDOW *win)
{
    werase(win);
    wprintw(win,
        " Q to stop simulation\tSPACE to pause simulation\tENTER to show "
        "details ");
    wrefresh(win);
}

void header(WINDOW *win)
{
    char s[512];
    int col = getmaxx(win);
    werase(win);
    sprintf(s, "Time elapsed: %.2lf", time_elapsed());
    mvwprintw(win, 0, (col - strlen(s)) / 2, "%ss", s);
    wrefresh(win);
}

void content(WINDOW *win)
{
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "O conteudo vai ficar aqui: yay");
    wrefresh(win);
}
