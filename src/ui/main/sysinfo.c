#include "sysinfo.h"
#include "../../core2ui.h"
#include "../colors.h"
#include <curses.h>
#include <stdint.h>
#include <string.h>

void sysinfo_render(
    struct ui_sysinfo *ui_sysinfo, double ut, uint64_t time_elapsed);
void ioinfo_render(struct ui_sysinfo *ui_sysinfo);

struct ui_sysinfo ui_sysinfo_create(WINDOW *parent_win)
{
    struct ui_sysinfo tmp = {
        .win = derwin(stdscr,
            (getmaxy(parent_win) - 1) - ((getmaxy(parent_win) - 1) * 3 / 5),
            getmaxx(parent_win) * 3 / 5, (getmaxy(parent_win) - 1) * 3 / 5, 0),
        .syswin = derwin(
            tmp.win, getmaxy(tmp.win) - 2, (getmaxx(tmp.win) - 2) / 2, 1, 1),
        .menuio = derwin(tmp.win, getmaxy(tmp.win) - 3,
            (getmaxx(tmp.win) / 2) - 1, 2, getmaxx(tmp.syswin) + 2),
        .io_info = vector_create(sizeof(struct io_info)),
    };
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    box(tmp.win, 0, 0);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // paint white title line
    wmove(tmp.win, 1, getmaxx(tmp.syswin) + 2);
    wattron(tmp.win, COLOR_PAIR(CP_TITLE));
    while (getcurx(tmp.win) < getmaxx(tmp.win) - 1)
        waddch(tmp.win, ' ');
    wattroff(tmp.win, COLOR_PAIR(CP_TITLE));

    // Write system info title
    char *title_sem = "System Info";
    wmove(tmp.win, 0, ((getmaxx(tmp.win) / 2) - strlen(title_sem) - 2) / 2);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_RTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));
    waddstr(tmp.win, title_sem);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_LTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // Write I/O Request List title
    char *title_io = "I/O Request List";
    wmove(tmp.win, 0,
        (getmaxx(tmp.win) / 2)
            + ((getmaxx(tmp.win) / 2) - strlen(title_io) - 2) / 2);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_RTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));
    waddstr(tmp.win, title_io);
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    waddch(tmp.win, ACS_LTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // Debugging
    // box(tmp.syswin, 0, 0);
    // box(tmp.menuio, 0, 0);

    // put a line in the middle
    wattron(tmp.win, COLOR_PAIR(CP_LINE));
    mvwaddch(tmp.win, 0, getmaxx(tmp.win) / 2, ACS_TTEE);
    for (int i = 1; i < getmaxy(tmp.win) - 1; i++)
        mvwaddch(tmp.win, i, getmaxx(tmp.win) / 2, ACS_VLINE);
    mvwaddch(tmp.win, getmaxy(tmp.win) - 1, getmaxx(tmp.win) / 2, ACS_BTEE);
    wattroff(tmp.win, COLOR_PAIR(CP_LINE));

    // // General System Info
    // wattron(tmp.syswin, COLOR_PAIR(CP_TITLE));
    // wprintw(tmp.syswin, "System Information:\n");
    // wattroff(tmp.syswin, COLOR_PAIR(CP_TITLE));

    wprintw(tmp.syswin, "\nUT: ");
    getyx(tmp.syswin, tmp.pos[0].y, tmp.pos[0].x);
    wprintw(tmp.syswin, "\nTime Elapsed: ");
    getyx(tmp.syswin, tmp.pos[1].y, tmp.pos[1].x);
    wprintw(tmp.syswin, "\nMemory Usage: ");
    getyx(tmp.syswin, tmp.pos[2].y, tmp.pos[2].x);
    wprintw(tmp.syswin, "\nDisk Speed: ");
    getyx(tmp.syswin, tmp.pos[3].y, tmp.pos[3].x);
    wprintw(tmp.syswin, "\nDisk Track: ");
    getyx(tmp.syswin, tmp.pos[4].y, tmp.pos[4].x);

    // IO System Info
    wattron(tmp.win, COLOR_PAIR(CP_TITLE));
    mvwprintw(tmp.win, 1, getmaxx(tmp.syswin) + 2,
        " %5.5s %3.3s %5.5s %8.8s %3.3s ", "IORID", "PID", "TRACK", "SEEKTIME",
        "R/W");
    wattroff(tmp.win, COLOR_PAIR(CP_TITLE));

    wrefresh(tmp.win);
    return tmp;
};

void ui_sysinfo_render(
    struct ui_sysinfo *ui_sysinfo, double ut, uint64_t time_elapsed)
{
    sysinfo_render(ui_sysinfo, ut, time_elapsed);
    ioinfo_render(ui_sysinfo);
}

void sysinfo_render(
    struct ui_sysinfo *ui_sysinfo, double ut, uint64_t time_elapsed)
{

    // UT
    wmove(ui_sysinfo->syswin, ui_sysinfo->pos[0].y, ui_sysinfo->pos[0].x);
    wclrtoeol(ui_sysinfo->syswin);
    wprintw(ui_sysinfo->syswin, "%1.5lf", ut);

    // Time elapsed
    wmove(ui_sysinfo->syswin, ui_sysinfo->pos[1].y, ui_sysinfo->pos[1].x);
    wclrtoeol(ui_sysinfo->syswin);
    wprintw(ui_sysinfo->syswin, "%lu UT", time_elapsed);

    // Memory usage
    wmove(ui_sysinfo->syswin, ui_sysinfo->pos[2].y, ui_sysinfo->pos[2].x);
    wclrtoeol(ui_sysinfo->syswin);
    wprintw(ui_sysinfo->syswin, "%lu/%d KB",
        ui_sysinfo->sysio_info.memory_usage_kb, 1024 * 1024);

    // Disk speed
    wmove(ui_sysinfo->syswin, ui_sysinfo->pos[3].y, ui_sysinfo->pos[3].x);
    wclrtoeol(ui_sysinfo->syswin);
    wprintw(ui_sysinfo->syswin, "%lu RPM", ui_sysinfo->sysio_info.speed);

    // Disk track
    wmove(ui_sysinfo->syswin, ui_sysinfo->pos[4].y, ui_sysinfo->pos[4].x);
    wclrtoeol(ui_sysinfo->syswin);
    wprintw(ui_sysinfo->syswin, "%u/%u", ui_sysinfo->sysio_info.track,
        ui_sysinfo->sysio_info.track_total);

    wrefresh(ui_sysinfo->syswin);
}

void ioinfo_render(struct ui_sysinfo *ui_sysinfo)
{
    werase(ui_sysinfo->menuio);

    for (size_t i = 0; i < ui_sysinfo->io_info.length; i++) {
        struct io_info *info = vector_get(&ui_sysinfo->io_info, i);
        if (info->io_id == ui_sysinfo->sysio_info.running_io_id) {
            wattron(ui_sysinfo->menuio, COLOR_PAIR(CP_LIST_ACTIVE));
        }

        mvwprintw(ui_sysinfo->menuio, i, 0,
            " "
            "  %3.3d %3.3d %5.5lu    %5.5d   %c"
            " ",
            info->io_id, info->process_id, info->track, info->seektime,
            info->rw ? 'W' : 'R');
        while (getcury(ui_sysinfo->menuio) < (int)i + 1)
            waddch(ui_sysinfo->menuio, ' ');
        wattroff(ui_sysinfo->menuio, COLOR_PAIR(CP_LIST_ACTIVE));
    }
    wrefresh(ui_sysinfo->menuio);
}

void ui_sysinfo_destroy(struct ui_sysinfo *ui_sysinfo)
{
    werase(ui_sysinfo->win);
    wrefresh(ui_sysinfo->win);
    vector_destroy(&ui_sysinfo->io_info);
    delwin(ui_sysinfo->menuio);
    delwin(ui_sysinfo->syswin);
}
