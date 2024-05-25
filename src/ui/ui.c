#include "ui.h"
#include "../core2ui.h"
#include "main/details.h"
#include "main/footer.h"
#include "main/header.h"
#include "main/process.h"
#include <curses.h>
#include <menu.h>
#include <stdint.h>
#include <time.h>

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
    struct ui_process ui_process = ui_process_create(stdscr);
    struct ui_details ui_details = ui_details_create(stdscr);
    struct ui_footer ui_footer = ui_footer_create(stdscr);
    // TODO: kernel_init()

    ui_footer_render(&ui_footer);

    clock_t it_clock = clock();
    while (true) {
        get_proc_info(&ui_process.proc_info);
        get_sem_info(&ui_details.sem_info, ui_process.highlight);
        get_page_info(&ui_details.page_info, ui_process.highlight);

        // Fix last option of menu if length changes
        if (ui_process.highlight > (int)ui_process.proc_info.length)
            ui_process.highlight = ui_process.proc_info.length;

        ui_render_header(&ui_header, ut, time_elapsed);
        ui_process_render(&ui_process);
        ui_details_render(&ui_details);
        switch (getch()) {
        case KEY_DOWN:
            // scroll down
            if (ui_process.highlight + 1 < (int)ui_process.proc_info.length)
                ++ui_process.highlight;
            break;
        case KEY_UP:
            // scroll up
            if (ui_process.highlight - 1 >= 0)
                --ui_process.highlight;
            break;
        case '+':
            // increase time per ut
            ut += ut_change;
            break;
        case '-':
            // decrease time per ut
            if (ut - ut_change >= 0)
                ut -= ut_change;
            break;
        case ' ':
            // toggle pause
            paused = !paused;
            break;
        case 'c':
            // create process
            // TODO: create_process(path) and create panel
            echo();
            (void)ui_footer_ask_path(&ui_footer);
            noecho();
            ui_footer_render(&ui_footer);
            break;
        case 'q':
            // Quit
            goto end;
        }

        // Check if it's time to run the next kernel operation
        if ((((double)(clock() - it_clock)) / CLOCKS_PER_SEC) >= ut
            && !paused) {
            ++time_elapsed;
            it_clock = clock();
            // TODO: kernel_run();
        }
    };
end:
    // TODO: kernel_shutdown()
    ui_destroy_header(&ui_header);
    ui_process_destroy(&ui_process);
    ui_details_destroy(&ui_details);
    ui_footer_destroy(&ui_footer);
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
