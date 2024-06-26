#include "ui.h"
#include "../core/kernel.h"
#include "../core2ui.h"
#include "colors.h"
#include "main/details.h"
#include "main/footer.h"
#include "main/process.h"
#include "main/sysinfo.h"
#include <curses.h>
#include <menu.h>
#include <stdint.h>
#include <time.h>

#define UT_CHANGE_INITIAL 0.0001
#define UT_MIN 0.001

#define TIME_ELAPSED(clk) (((double)(clock() - clk)) / CLOCKS_PER_SEC)
#define UT_CHANGE_ACELERATION 1.011

void ui_create_stdscr();
void ui_destroy_stdscr();

uint64_t time_elapsed = 0;
bool paused = true;

void run_curses()
{
    double ut = UT_MIN;
    double ut_change = UT_CHANGE_INITIAL;

    ui_create_stdscr();
    struct ui_process ui_process = ui_process_create(stdscr);
    struct ui_details ui_details = ui_details_create(stdscr);
    struct ui_footer ui_footer = ui_footer_create(stdscr);
    struct ui_sysinfo ui_sysinfo = ui_sysinfo_create(stdscr);
    kernel_init();

    ui_footer_render(&ui_footer);

    clock_t it_clock = clock();
    clock_t last_speed_change = clock();
    bool next_instr = false;
    while (true) {
        // Check if it's time to run the next kernel operation
        if ((TIME_ELAPSED(it_clock) >= ut && !paused) || next_instr) {
            next_instr = false;
            ++time_elapsed;
            it_clock = clock();
            kernel_run();
        }

        if (TIME_ELAPSED(last_speed_change) > 0.7)
            ut_change = UT_CHANGE_INITIAL;

        get_proc_info(&ui_process.proc_info);

        // Fix last option of menu if length changes
        if (ui_process.proc_info.length != 0
            && ui_process.highlight > (int)ui_process.proc_info.length - 1)
            ui_process.highlight = ui_process.proc_info.length - 1;

        get_sem_info(&ui_details.sem_info, ui_process.highlight);
        get_page_info(&ui_details.page_info, ui_process.highlight);
        get_sysioinfo(&ui_sysinfo.io_info, &ui_sysinfo.sysio_info);

        ui_sysinfo_render(&ui_sysinfo, ut, time_elapsed);
        ui_process_render(&ui_process);
        ui_details_render(&ui_details);

        switch (getch()) {
        case 'n':
            next_instr = true;
            break;
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
            last_speed_change = clock();
            ut_change *= UT_CHANGE_ACELERATION;
            ut += ut_change;
            break;
        case '-':
            // decrease time per ut
            last_speed_change = clock();
            if (ut - ut_change >= UT_MIN) {
                ut_change *= UT_CHANGE_ACELERATION;
                ut -= ut_change;
            } else {
                ut = UT_MIN;
            }
            break;
        case '0':
            ut = UT_MIN;
            break;
        case ' ':
            // toggle pause
            paused = !paused;
            break;
        case 'c':
            echo();
            syscall(PROCESS_CREATE, ui_footer_ask_path(&ui_footer));
            noecho();
            ui_footer_render(&ui_footer);
            break;
        case 'q':
            // Quit
            goto end;
        }
    };
end:
    kernel_shutdown();
    ui_process_destroy(&ui_process);
    ui_details_destroy(&ui_details);
    ui_footer_destroy(&ui_footer);
    ui_sysinfo_destroy(&ui_sysinfo);
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
    start_color();
    init_pair(CP_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(CP_LIST_ITEM, COLOR_WHITE, COLOR_BLACK);
    init_pair(CP_LIST_HIGHLIGHT, COLOR_GREEN, COLOR_BLACK);
    init_pair(CP_ACTIVE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CP_LINE, COLOR_BLUE, COLOR_BLACK);
    init_pair(CP_TITLE, COLOR_BLACK, COLOR_WHITE);
}
void ui_destroy_stdscr() { endwin(); }
