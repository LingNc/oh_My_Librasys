#include "ui/tui/components/menu.h"

#include <curses.h>

#include "ui/tui/key.h"
#include "ui/tui/theme.h"
#include "ui/tui/window.h"
#include "ui/tui/width.h"

#define MENU_DEFAULT_HINT "↑↓/jk 选择  Enter 确认  q 返回  数字直选"

int tui_menu(const tui_menu_spec *spec) {
    if (!spec || !spec->items || spec->count <= 0) return -1;

    const char *hint = spec->hint ? spec->hint : MENU_DEFAULT_HINT;
    int hl = spec->initial;
    if (hl < 0) hl = 0;
    if (hl >= spec->count) hl = spec->count - 1;

    for (;;) {
        ui_frame f;
        if (!ui_frame_begin(spec->title, hint, &f)) {
            /* 终端过小：等一次 resize 或任意键后重试 */
            tui_key k0 = tui_read_key();
            if (tui_is_cancel(k0)) return -1;
            continue;
        }

        int y = f.y0;
        if (spec->pre_info) {
            y = spec->pre_info(spec->ctx, &f, y);
        }

        /* 居中模式：按最长选项计算内容块宽度，整体水平居中 */
        int xoff = 0;
        int block_w = f.w;
        if (spec->center) {
            int maxw = 0;
            for (int i = 0; i < spec->count; ++i) {
                int w = ui_utf8_width(spec->items[i]);
                if (w > maxw) maxw = w;
            }
            block_w = maxw + 2;
            if (block_w > f.w) block_w = f.w;
            xoff = (f.w - block_w) / 2;
            if (xoff < 0) xoff = 0;
        }

        for (int i = 0; i < spec->count && y < f.y1; ++i, ++y) {
            bool sel = (i == hl);
            int attr = sel ? theme_attr(UI_PAIR_HILITE) : theme_attr(UI_PAIR_NORMAL);

            if (sel) {
                ui_clear_span(y, f.x0 + xoff, block_w);
            }
            ui_puts(y, f.x0 + xoff, 2, sel ? ">" : " ", attr);
            ui_puts(y, f.x0 + xoff + 2, block_w - 2, spec->items[i], attr);

            if (sel) {
                /* 用高亮色补满内容块，形成选中条 */
                int used = 2 + ui_utf8_width(spec->items[i]);
                if (used < block_w) {
                    attron(attr);
                    mvhline(y, f.x0 + xoff + used, ' ', block_w - used);
                    attroff(attr);
                }
            }
        }

        if (spec->post_info) {
            int y2 = y + 1;
            if (y2 < f.y1) spec->post_info(spec->ctx, &f, y2);
        }

        ui_frame_end();

        tui_key k = tui_read_key();

        if (k.type == TUI_KEY_RESIZE) continue;
        if (spec->enable_command && tui_is_char(k, L'/')) return TUI_RESULT_COMMAND;
        if (tui_is_cancel(k)) return -1;
        if (tui_is_enter(k)) return hl;
        if (tui_is_prev(k)) {
            hl = (hl - 1 + spec->count) % spec->count;
            continue;
        }
        if (tui_is_next(k)) {
            hl = (hl + 1) % spec->count;
            continue;
        }
        if (k.type == TUI_KEY_HOME) { hl = 0; continue; }
        if (k.type == TUI_KEY_END)  { hl = spec->count - 1; continue; }
        if (tui_key_printable(k) && k.ch >= L'1' && k.ch <= L'9') {
            int idx = (int)(k.ch - L'1');
            if (idx < spec->count) return idx;
        }
    }
}
