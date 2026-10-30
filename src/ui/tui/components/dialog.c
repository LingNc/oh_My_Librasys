#include "ui/tui/components/dialog.h"

#include <curses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui/tui/key.h"
#include "ui/tui/theme.h"
#include "ui/tui/window.h"
#include "ui/tui/width.h"

#define DIALOG_MAX_LINES 20
#define DIALOG_LINE_MAX  512

typedef struct {
    int w, h;             /* 外框宽高（含边框） */
    int y, x;             /* 左上角 */
    char lines[DIALOG_MAX_LINES][DIALOG_LINE_MAX];
    int nlines;
} dialog_box;

/* 简单按列宽折行（中文不会劈开） */
static int wrap_text(const char *text, int max_cols,
                     char out[DIALOG_MAX_LINES][DIALOG_LINE_MAX]) {
    int n = 0;
    if (max_cols < 2) return 0;

    wchar_t *w = ui_wcs_from_utf8(text);
    if (!w) return 0;

    size_t i = 0;
    while (w[i] && n < DIALOG_MAX_LINES) {
        int used = 0;
        size_t start = i;
        while (w[i]) {
            if (w[i] == L'\n') {
                ++i;
                break;
            }
            int cw = ui_wcwidth(w[i]);
            if (used + cw > max_cols) break;
            used += cw;
            ++i;
        }
        size_t len = i - start;
        if (len == 0 && w[i]) { ++i; len = 1; }

        wchar_t tmp[DIALOG_LINE_MAX];
        if (len >= DIALOG_LINE_MAX) len = DIALOG_LINE_MAX - 1;
        wmemcpy(tmp, w + start, len);
        tmp[len] = L'\0';

        char *u = ui_utf8_from_wcs(tmp);
        if (u) {
            ui_utf8_copy(out[n], DIALOG_LINE_MAX, u);
            free(u);
        } else {
            out[n][0] = '\0';
        }
        ++n;
    }

    free(w);
    return n;
}

static void compute_geometry(dialog_box *box) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int content_w = 0;
    for (int i = 0; i < box->nlines; ++i) {
        int cw = ui_utf8_width(box->lines[i]);
        if (cw > content_w) content_w = cw;
    }

    box->w = content_w + 6;
    if (box->w > cols - 4) box->w = cols - 4;
    if (box->w < 20) box->w = 20;

    box->h = box->nlines + 4;
    if (box->h > rows - 2) box->h = rows - 2;

    box->y = (rows - box->h) / 2;
    box->x = (cols - box->w) / 2;
    if (box->y < 0) box->y = 0;
    if (box->x < 0) box->x = 0;
}

static void draw_box(const dialog_box *box, const char *title, const char *footer,
                     int title_attr) {
    /* 模态对话框：先清屏再画，避免上一个对话框的边框残留。
     * 对话框关闭后调用方会重绘自己的视图。 */
    erase();

    WINDOW *w = derwin(stdscr, box->h, box->w, box->y, box->x);
    if (!w) return;

    /* 内容区可用宽度：左右各留 1 列内边距 + 两侧边框 */
    int inner = box->w - 5;
    if (inner < 1) inner = 1;

    /* 先画内容并按列宽裁剪，避免内容越过右边框 */
    for (int i = 0; i < box->nlines && 2 + i < box->h - 2; ++i) {
        char clipped[DIALOG_LINE_MAX];
        ui_fit_utf8(box->lines[i], inner, clipped, sizeof(clipped));
        mvwaddstr(w, 2 + i, 3, clipped);
    }

    if (footer && footer[0]) {
        int fw = ui_utf8_width(footer);
        int fx = box->w - 3 - fw;
        if (fx < 1) fx = 1;
        wattron(w, theme_attr(UI_PAIR_HINT));
        mvwaddstr(w, box->h - 2, fx, footer);
        wattroff(w, theme_attr(UI_PAIR_HINT));
    }

    /* 边框最后画，保证任何情况下都不会被内容遮挡 */
    box(w, 0, 0);

    if (title && title[0]) {
        char t[256];
        snprintf(t, sizeof(t), " %s ", title);
        wattron(w, title_attr);
        mvwaddstr(w, 0, 2, t);
        wattroff(w, title_attr);
    }

    wnoutrefresh(w);
    delwin(w);
    doupdate();
}

static void vmessage(const char *title, const char *footer, int title_attr,
                     const char *fmt, va_list ap) {
    char text[DIALOG_LINE_MAX * 2];
    vsnprintf(text, sizeof(text), fmt, ap);

    dialog_box box;
    memset(&box, 0, sizeof(box));
    box.nlines = wrap_text(text, 60, box.lines);
    if (box.nlines == 0) {
        snprintf(box.lines[0], DIALOG_LINE_MAX, "%.500s", text);
        box.nlines = 1;
    }
    compute_geometry(&box);
    draw_box(&box, title, footer, title_attr);
}

void tui_info(const char *title, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vmessage(title, "[ Enter 关闭 ]", theme_attr(UI_PAIR_TITLE), fmt, ap);
    va_end(ap);

    for (;;) {
        tui_key k = tui_read_key();
        if (k.type == TUI_KEY_RESIZE) continue;
        break;
    }
}

void tui_error(const char *title, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vmessage(title ? title : "错误", "[ Enter 关闭 ]", theme_attr(UI_PAIR_ERROR), fmt, ap);
    va_end(ap);

    for (;;) {
        tui_key k = tui_read_key();
        if (k.type == TUI_KEY_RESIZE) continue;
        break;
    }
}

bool tui_confirm(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vmessage("确认", "[ y 确认   n/Esc 取消 ]", theme_attr(UI_PAIR_TITLE), fmt, ap);
    va_end(ap);

    for (;;) {
        tui_key k = tui_read_key();
        if (k.type == TUI_KEY_RESIZE) continue;
        if (tui_is_char(k, L'y') || tui_is_char(k, L'Y')) return true;
        if (tui_is_char(k, L'n') || tui_is_char(k, L'N')) return false;
        if (tui_is_enter(k)) return true;
        if (k.type == TUI_KEY_ESC) return false;
    }
}

void tui_toast(const char *fmt, ...) {
    char text[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    char shown[512];
    ui_fit_utf8(text, cols - 8, shown, sizeof(shown));

    int w = ui_utf8_width(shown) + 4;
    int y = rows - 1;
    int x = cols - w - 2;
    if (x < 1) x = 1;

    attron(theme_attr(UI_PAIR_OK));
    mvhline(y, x, ' ', w);
    mvaddstr(y, x + 2, shown);
    attroff(theme_attr(UI_PAIR_OK));
    wnoutrefresh(stdscr);
    doupdate();

    napms(800);
}

void tui_progress(const char *title, const char *stage, size_t done, size_t total) {
    ui_frame frame;
    if (!ui_frame_begin(title ? title : "正在导入", "请稍候…", &frame)) {
        ui_frame_end();
        return;
    }

    if (total == 0) total = 1;
    if (done > total) done = total;

    ui_puts(frame.y0, frame.x0, frame.w, stage ? stage : "处理中",
            theme_attr(UI_PAIR_TITLE));

    int bar_width = frame.w - 2;
    if (bar_width > 60) bar_width = 60;
    if (bar_width < 10) bar_width = frame.w;
    int bar_y = frame.y0 + 2;
    if (bar_y >= frame.y1) bar_y = frame.y1 - 1;
    int filled = (int)((double)done * (double)bar_width / (double)total);

    ui_puts(bar_y, frame.x0, 1, "[", theme_attr(UI_PAIR_HINT));
    for (int i = 0; i < bar_width; ++i) {
        wchar_t cell = i < filled ? L'█' : L'░';
        ui_puts_w(bar_y, frame.x0 + 1 + i, 1, &cell,
                  i < filled ? theme_attr(UI_PAIR_OK) : theme_attr(UI_PAIR_HINT));
    }
    ui_puts(bar_y, frame.x0 + bar_width + 1, 1, "]", theme_attr(UI_PAIR_HINT));

    char count[96];
    snprintf(count, sizeof(count), "%zu / %zu  (%zu%%)", done, total,
             (done * 100) / total);
    ui_puts(bar_y + 1, frame.x0, frame.w, count, theme_attr(UI_PAIR_NORMAL));
    ui_frame_end();
}
