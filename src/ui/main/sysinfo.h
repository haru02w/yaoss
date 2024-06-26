#pragma once
#include "../../core2ui.h"
#include "../../util/vector.h"
#include "curses.h"

struct ui_sysinfo {
    WINDOW *win;
    WINDOW *syswin;
    WINDOW *menuio;
    struct {
        int y, x;
    } pos[5];
    struct sysio_info sysio_info;
    struct vector io_info;
};

struct ui_sysinfo ui_sysinfo_create(WINDOW *parent_win);
void ui_sysinfo_render(
    struct ui_sysinfo *ui_sysinfo, double ut, uint64_t time_elapsed);
void ui_sysinfo_destroy(struct ui_sysinfo *ui_sysinfo);
