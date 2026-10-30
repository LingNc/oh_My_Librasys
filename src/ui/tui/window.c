#include "ui/tui/window.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "ui/tui/theme.h"
#include "ui/tui/width.h"

static void draw_tiny_notice(void) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    erase();
    ui_puts_center(rows / 2, 0, cols, "终端窗口过小，请放大到至少 60x16", theme_attr(UI_PAIR_ERROR));
    ui_puts_center(rows / 2 + 2, 0, cols, "调整后界面会自动重绘", theme_attr(UI_PAIR_HINT));
    refresh();
}

bool ui_frame_begin(const char *title, const char *status, ui_frame *f) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (f) {
        f->rows = rows;
        f->cols = cols;
        f->y0 = 1;
        f->x0 = 2;
        f->y1 = rows - 1;
        f->x1 = cols - 2;
        f->w  = f->x1 - f->x0;
        f->h  = f->y1 - f->y0;
    }

    if (rows < UI_MIN_ROWS || cols < UI_MIN_COLS) {
        draw_tiny_notice();
        return false;
    }

    erase();
    box(stdscr, 0, 0);

    if (title) {
        char buf[256];
        snprintf(buf, sizeof(buf), " %s ", title);
        ui_puts(0, 2, cols - 4, buf, theme_attr(UI_PAIR_TITLE));
    }
    if (status) {
        ui_puts(rows - 1, 2, cols - 4, status, theme_attr(UI_PAIR_HINT));
    }
    return true;
}

void ui_frame_end(void) {
    wnoutrefresh(stdscr);
    doupdate();
}

void ui_puts_w(int y, int x, int maxw, const wchar_t *s, int attr) {
    if (!s || maxw == 0) return;

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (y < 0 || y >= rows) return;
    if (maxw < 0 || x + maxw > cols) maxw = cols - x;
    if (maxw <= 0) return;

    if (attr) attron(attr);

    int used = 0;
    for (const wchar_t *p = s; *p; ++p) {
        int cw = ui_wcwidth(*p);
        if (used + cw > maxw) break;
        if (mvaddnwstr(y, x + used, p, 1) == ERR) break;
        used += cw;
    }

    if (attr) attroff(attr);
}

void ui_puts(int y, int x, int maxw, const char *utf8, int attr) {
    if (!utf8) return;
    wchar_t *w = ui_wcs_from_utf8(utf8);
    if (!w) return;
    ui_puts_w(y, x, maxw, w, attr);
    free(w);
}

void ui_puts_center_w(int y, int x, int maxw, const wchar_t *s, int attr) {
    if (!s) return;
    int width = ui_wcswidth(s);
    int off = (maxw - width) / 2;
    if (off < 0) off = 0;
    ui_puts_w(y, x + off, maxw - off, s, attr);
}

void ui_puts_center(int y, int x, int maxw, const char *utf8, int attr) {
    if (!utf8) return;
    wchar_t *w = ui_wcs_from_utf8(utf8);
    if (!w) return;
    ui_puts_center_w(y, x, maxw, w, attr);
    free(w);
}

void ui_clear_span(int y, int x, int cols) {
    if (cols <= 0) return;
    int rows, screen_cols;
    getmaxyx(stdscr, rows, screen_cols);
    if (y < 0 || y >= rows) return;
    if (x + cols > screen_cols) cols = screen_cols - x;
    if (cols <= 0) return;
    mvhline(y, x, ' ', cols);
}

void ui_hline_w(int y, int x, int cols, wchar_t ch) {
    if (cols <= 0) return;
    int rows, screen_cols;
    getmaxyx(stdscr, rows, screen_cols);
    if (y < 0 || y >= rows) return;
    if (x + cols > screen_cols) cols = screen_cols - x;
    for (int i = 0; i < cols; ++i) {
        if (mvaddnwstr(y, x + i, &ch, 1) == ERR) break;
    }
}
