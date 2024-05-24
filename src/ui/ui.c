#include "ui.h"
#include "../core2ui.h"
#include "main/details.h"
#include "main/footer.h"
#include "main/header.h"
#include "main/process.h"
#include <curses.h>
#include <menu.h>
#include <stdint.h>

void ui_create_stdscr();
void ui_destroy_stdscr();

uint64_t time_elapsed = 0;
bool paused = true;

void run_curses()
{
    double ut = 0.0001;
    double ut_change = ut * 0.1;

    ui_create_stdscr();
    struct ui_header ui_header = ui_create_header(stdscr);
    struct ui_process ui_process = ui_create_process(stdscr);
    struct ui_footer ui_footer = ui_create_footer(stdscr);
    struct ui_details ui_details = ui_create_details(stdscr);

    ui_render_footer(&ui_footer);

    for (;;) {
        ui_process.proc_info = get_processes_info();

        if (ui_process.highlight > (int)ui_process.proc_info->length)
            ui_process.highlight = ui_process.proc_info->length;

        ui_render_header(&ui_header, ut, time_elapsed);
        ui_render_process(&ui_process);
        switch (getch()) {
        case KEY_DOWN:
            if (ui_process.highlight + 1 < (int)ui_process.proc_info->length)
                ++ui_process.highlight;
            break;
        case KEY_UP:
            if (ui_process.highlight - 1 >= 0)
                --ui_process.highlight;
            break;
        case '+':
            ut += ut_change;
            break;
        case '-':
            if (ut - ut_change >= 0)
                ut -= ut_change;
            break;
        case ' ':
            paused = !paused;
            break;
        case 'c':
            echo();
            // TODO: create_process(path) and create panel
            (void)ui_ask_path_footer(&ui_footer);
            noecho();
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
    ui_destroy_details(&ui_details);
    ui_destroy_footer(&ui_footer);
    ui_destroy_stdscr();
}

void ui_create_stdscr()
{
    /* Initialization */
    initscr(); // Start curses mode
    cbreak(); // disable line buffering except CTRL+C
    noecho(); // disable showing input text on screen
    keypad(stdscr, TRUE); // get F1-F12 and arrow keys as well
    nodelay(stdscr, TRUE); // don't block thread when getting input
    curs_set(0); // disable cursor
}
void ui_destroy_stdscr() { endwin(); }
