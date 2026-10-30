#include "ui/tui/components/table.h"

#include <curses.h>
#include <stdio.h>
#include <string.h>

#include "ui/tui/key.h"
#include "ui/tui/theme.h"
#include "ui/tui/window.h"
#include "ui/tui/width.h"

#define TABLE_CELL_MAX 256
#define TABLE_DEFAULT_PAGE 10
#define TABLE_DEFAULT_HINT "↑↓/jk 移动  ←→/hl 翻页  Enter 查看  q 返回"

static size_t g_default_page = TABLE_DEFAULT_PAGE;
static bool g_pending_page = false;
static size_t g_pending_page_value = 0;

size_t tui_table_default_page_size(void) {
    return g_default_page;
}

void tui_table_set_default_page_size(size_t size) {
    if (size > 0) g_default_page = size;
}

void tui_table_request_page(size_t page) {
    g_pending_page = true;
    g_pending_page_value = page;
}

/* 画一行表头 */
static void draw_header(const tui_table_spec *spec, const ui_frame *f, int y) {
    if (!spec->headers || y >= f->y1) return;

    int x = f->x0;
    for (size_t c = 0; c < spec->ncols; ++c) {
        int cw = spec->col_widths[c];
        if (x + cw > f->x1) cw = f->x1 - x;
        if (cw <= 0) break;
        ui_puts(y, x, cw, spec->headers[c], theme_attr(UI_PAIR_TITLE) | A_BOLD);
        x += spec->col_widths[c] + 1;
    }
}

/* 画一行数据，sel 为 true 时整行高亮 */
static void draw_row(const tui_table_spec *spec, const ui_frame *f, int y,
                     size_t row, bool sel) {
    int attr = sel ? theme_attr(UI_PAIR_HILITE) : theme_attr(UI_PAIR_NORMAL);

    if (sel) {
        ui_clear_span(y, f->x0, f->w);
    }
    if (attr) attron(attr);
    mvhline(y, f->x0, ' ', f->w);
    if (attr) attroff(attr);

    int x = f->x0;
    for (size_t c = 0; c < spec->ncols; ++c) {
        int cw = spec->col_widths[c];
        if (x + cw > f->x1) cw = f->x1 - x;
        if (cw <= 0) break;

        char cell[TABLE_CELL_MAX];
        cell[0] = '\0';
        spec->row_text(spec->ctx, row, c, cell, sizeof(cell));

        char clipped[TABLE_CELL_MAX];
        /* 先按列宽裁剪，再用空格补齐，保证后续列位置稳定 */
        ui_fit_utf8(cell, cw, clipped, sizeof(clipped));

        ui_puts(y, x, cw, clipped, attr);

        int used = ui_utf8_width(clipped);
        if (used < cw) {
            if (attr) attron(attr);
            mvhline(y, x + used, ' ', cw - used);
            if (attr) attroff(attr);
        }
        x += spec->col_widths[c] + 1;
    }
}

long tui_table(const tui_table_spec *spec) {
    if (!spec || !spec->row_count || !spec->row_text || spec->ncols == 0) return -1;

    const char *hint = spec->hint ? spec->hint : TABLE_DEFAULT_HINT;
    size_t page_size = spec->page_size ? spec->page_size : tui_table_default_page_size();

    size_t total = spec->row_count(spec->ctx);
    size_t page = 0;
    size_t hl = 0;              /* 页内高亮 */
    size_t total_pages = (total + page_size - 1) / page_size;
    if (total_pages == 0) total_pages = 1;

    for (;;) {
        total = spec->row_count(spec->ctx);
        total_pages = (total + page_size - 1) / page_size;
        if (total_pages == 0) total_pages = 1;
        if (page >= total_pages) page = total_pages - 1;

        /* 命令面板 /page 请求的跳转，只生效一次 */
        if (g_pending_page) {
            page_size = spec->page_size ? spec->page_size : tui_table_default_page_size();
            total_pages = (total + page_size - 1) / page_size;
            if (total_pages == 0) total_pages = 1;
            page = g_pending_page_value < total_pages ? g_pending_page_value : total_pages - 1;
            hl = 0;
            g_pending_page = false;
        }
        ui_frame f;
        if (!ui_frame_begin(spec->title, hint, &f)) {
            tui_key k0 = tui_read_key();
            if (tui_is_cancel(k0)) return -1;
            continue;
        }

        char info[128];
        snprintf(info, sizeof(info), " 共 %zu 条  第 %zu/%zu 页 ",
                 total, page + 1, total_pages);
        ui_puts(f.y1 - 1, f.x1 - ui_utf8_width(info), f.w, info, theme_attr(UI_PAIR_HINT));

        int y = f.y0;
        draw_header(spec, &f, y);
        size_t row_width = 0;
        for (size_t c = 0; c < spec->ncols; ++c) row_width += (size_t)spec->col_widths[c] + 1;
        if (row_width > 0) {
            ui_hline_w(y + 1, f.x0, (int)(row_width - 1), L'─');
        }
        y += 2;

        size_t base = page * page_size;
        size_t shown = 0;
        for (size_t i = 0; i < page_size && base + i < total; ++i) {
            if (y >= f.y1) break;
            draw_row(spec, &f, y, base + i, i == hl);
            ++y;
            ++shown;
        }

        if (shown == 0) {
            ui_puts(f.y0 + 3, f.x0, f.w, "（暂无数据）", theme_attr(UI_PAIR_HINT));
        } else if (hl >= shown) {
            hl = shown - 1;
        }

        ui_frame_end();

        tui_key k = tui_read_key();

        if (k.type == TUI_KEY_RESIZE) continue;
        if (spec->enable_command && tui_is_char(k, L'/')) return TUI_RESULT_COMMAND;
        if (tui_is_cancel(k)) return -1;
        if (tui_is_prev(k)) {
            if (hl > 0) --hl;
            else if (page > 0) { --page; hl = page_size - 1; }
            continue;
        }
        if (tui_is_next(k)) {
            if (hl + 1 < shown) ++hl;
            else if (page + 1 < total_pages) { ++page; hl = 0; }
            continue;
        }
        if (tui_is_prev_page(k)) {
            if (page > 0) { --page; hl = 0; }
            continue;
        }
        if (tui_is_next_page(k)) {
            if (page + 1 < total_pages) { page += 1; hl = 0; }
            continue;
        }
        if (k.type == TUI_KEY_HOME) { page = 0; hl = 0; continue; }
        if (k.type == TUI_KEY_END)  {
            page = total_pages - 1;
            size_t last = total - page * page_size;
            hl = last ? last - 1 : 0;
            continue;
        }
        if (tui_is_enter(k)) {
            size_t abs_row = base + hl;
            if (abs_row < total) {
                if (spec->on_enter) {
                    if (!spec->on_enter(spec->ctx, abs_row)) return (long)abs_row;
                } else {
                    return (long)abs_row;
                }
            }
            continue;
        }
        if (tui_key_printable(k) && k.ch >= L'1' && k.ch <= L'9') {
            size_t idx = (size_t)(k.ch - L'1');
            if (idx < shown) { hl = idx; }
            continue;
        }
    }
}
