#include "ui.h"
#include "main/footer.h"
#include "main/header.h"
#include "main/process.h"
#include <curses.h>
#include <math.h>
#include <menu.h>
#include <stdint.h>

void process_menu_handle(WINDOW *win, MENU *menu, int keycode);

bool paused = true;
uint64_t time_elapsed = 0;

#define UT_CHANGE 0.00001
void start_ui()
{
    double ut = 0.0001;

    /* Initialization */
    initscr(); // Start curses mode
    cbreak(); // disable line buffering except CTRL+C
    noecho(); // disable showing input text on screen
    keypad(stdscr, TRUE); // get F1-F12 and arrow keys as well
    nodelay(stdscr, TRUE); // don't block thread when getting input
    curs_set(0); // disable cursor

    int row = getmaxy(stdscr);
    int col = getmaxx(stdscr);

    struct ui_header ui_header = ui_create_header(stdscr);
    struct ui_process ui_process = ui_create_process(stdscr);
    struct ui_footer ui_footer = ui_create_footer(stdscr);

    // TODO:
    WINDOW *win_process_details
        = derwin(stdscr, row - 2, col / 2, 1, (col / 2));
    box(win_process_details, 0, 0);

    ui_render_footer(&ui_footer);

    for (;;) {
        ui_render_header(&ui_header, ut, 4324242, time_elapsed);
        ui_render_process(&ui_process); // TODO: inside loop
        switch (getch()) {
        case KEY_DOWN:
            // TODO:
            break;
        case KEY_UP:
            // TODO:
            break;
        case '+':
            if (ut + UT_CHANGE > INFINITY)
                break;
            ut += 0.00001;
            break;
        case '-':
            if (ut - UT_CHANGE < 0)
                break;
            ut -= 0.00001;
            break;
        case ' ':
            paused = !paused;
            break;
        case 'c':
            echo();
            char *path = ui_ask_path_footer(&ui_footer);
            noecho();
            // TODO: create_process(path)
            ui_render_footer(&ui_footer);
            break;
        case 'q':
            goto end;
        }

        sleep(ut);
        time_elapsed += !paused;
    };
end:
    ui_destroy_header(&ui_header);
    ui_destroy_process(&ui_process);
    ui_destroy_footer(&ui_footer);
    endwin();
}
