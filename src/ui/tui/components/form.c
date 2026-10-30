#include "ui/tui/components/form.h"

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "ui/tui/key.h"
#include "ui/tui/theme.h"
#include "ui/tui/window.h"
#include "ui/tui/width.h"

#define FORM_MAX_CHARS 256
#define FORM_DEFAULT_HINT "Tab/↑↓ 切换  Enter 确认  Esc 取消"

typedef struct {
    wchar_t text[FORM_MAX_CHARS];
    size_t len;
} form_buf;

static void buf_load(form_buf *fb, const char *utf8) {
    fb->len = 0;
    if (!utf8) { fb->text[0] = L'\0'; return; }

    wchar_t *w = ui_wcs_from_utf8(utf8);
    if (!w) { fb->text[0] = L'\0'; return; }

    size_t n = wcslen(w);
    if (n >= FORM_MAX_CHARS) n = FORM_MAX_CHARS - 1;
    wmemcpy(fb->text, w, n);
    fb->text[n] = L'\0';
    fb->len = n;
    free(w);
}

static void buf_store(const form_buf *fb, char *dst, size_t cap) {
    if (!dst || cap == 0) return;
    char *u = ui_utf8_from_wcs(fb->text);
    if (!u) { dst[0] = '\0'; return; }
    ui_utf8_copy(dst, cap, u);
    free(u);
}

static bool valid_input_char(const tui_field *field, wchar_t ch) {
    if (wcwidth(ch) < 0) return false;
    if (field->flags & TUI_FIELD_NUMERIC) {
        return (ch >= L'0' && ch <= L'9') || ch == L'-';
    }
    return true;
}

/* 计算标签列宽（统一对齐） */
static int label_width(const tui_field *fields, size_t n) {
    int w = 0;
    for (size_t i = 0; i < n; ++i) {
        int cw = ui_utf8_width(fields[i].label ? fields[i].label : "");
        if (cw > w) w = cw;
    }
    return w;
}

tui_form_result tui_form(const char *title, tui_field *fields, size_t nfields,
                         const char *submithint) {
    if (!fields || nfields == 0) return TUI_FORM_CANCEL;

    form_buf *bufs = calloc(nfields, sizeof(*bufs));
    if (!bufs) return TUI_FORM_CANCEL;

    for (size_t i = 0; i < nfields; ++i) {
        buf_load(&bufs[i], fields[i].buf);
    }

    size_t active = 0;
    char error[256] = {0};
    const char *hint = submithint ? submithint : FORM_DEFAULT_HINT;
    const int lw = label_width(fields, nfields);
    tui_form_result result = TUI_FORM_CANCEL;

    for (;;) {
        ui_frame f;
        if (!ui_frame_begin(title, hint, &f)) {
            tui_key k0 = tui_read_key();
            if (tui_is_cancel(k0)) break;
            continue;
        }

        int y = f.y0;
        int value_x = f.x0 + lw + 2;

        for (size_t i = 0; i < nfields && y < f.y1; ++i, ++y) {
            bool sel = (i == active);
            int lattr = sel ? theme_attr(UI_PAIR_HILITE) : theme_attr(UI_PAIR_NORMAL);

            /* 标签右对齐，格式 "标签:" */
            char label[160];
            snprintf(label, sizeof(label), "%s:",
                     fields[i].label ? fields[i].label : "");
            int lpad = lw + 1 - ui_utf8_width(label);
            if (lpad < 0) lpad = 0;
            if (lpad > 0) {
                ui_clear_span(y, f.x0, lpad);
            }
            ui_puts(y, f.x0 + lpad, lw + 2 - lpad, label, lattr);

            /* 输入值 */
            wchar_t masked[FORM_MAX_CHARS];
            const wchar_t *shown = bufs[i].text;
            if (fields[i].flags & TUI_FIELD_MASKED) {
                for (size_t k = 0; k < bufs[i].len && k + 1 < FORM_MAX_CHARS; ++k) {
                    masked[k] = L'*';
                }
                masked[bufs[i].len < FORM_MAX_CHARS ? bufs[i].len : FORM_MAX_CHARS - 1] = L'\0';
                shown = masked;
            }

            int avail = f.x1 - value_x;
            if (avail <= 0) break;

            if (sel) {
                ui_clear_span(y, value_x, avail);
            }
            ui_puts_w(y, value_x, avail, shown, theme_attr(UI_PAIR_NORMAL));

            if (sel) {
                /* 把光标放在文本末尾 */
                int col = value_x + ui_wcswidth(shown);
                if (col >= f.x1) col = f.x1 - 1;
                wmove(stdscr, y, col);
                curs_set(1);
            }
        }

        if (error[0] && y + 1 < f.y1) {
            ui_puts(y + 1, f.x0, f.w, error, theme_attr(UI_PAIR_ERROR));
        }

        ui_frame_end();

        tui_key k = tui_read_key();

        if (k.type == TUI_KEY_RESIZE) continue;

        if (k.type == TUI_KEY_ESC) break;

        if (k.type == TUI_KEY_TAB) {
            active = (active + 1) % nfields;
            error[0] = '\0';
            continue;
        }
        if (k.type == TUI_KEY_BTAB) {
            active = (active + nfields - 1) % nfields;
            error[0] = '\0';
            continue;
        }
        if (tui_is_backspace(k)) {
            if (bufs[active].len > 0) {
                bufs[active].text[--bufs[active].len] = L'\0';
            }
            error[0] = '\0';
            continue;
        }

        /* 注意：表单是文本输入，j/k/h/l/w/s 等字符必须当普通字符插入，
         * 只有方向键 / Tab 才允许切换字段。 */
        if (k.type == TUI_KEY_UP) {
            active = (active + nfields - 1) % nfields;
            error[0] = '\0';
            continue;
        }
        if (k.type == TUI_KEY_DOWN) {
            active = (active + 1) % nfields;
            error[0] = '\0';
            continue;
        }
        if (tui_is_enter(k)) {
            /* 校验所有字段 */
            bool ok = true;
            error[0] = '\0';
            for (size_t i = 0; i < nfields; ++i) {
                char tmp[FORM_MAX_CHARS * 4];
                buf_store(&bufs[i], tmp, sizeof(tmp));
                if (fields[i].validate) {
                    const char *msg = fields[i].validate(tmp);
                    if (msg) {
                        snprintf(error, sizeof(error), "%s: %s",
                                 fields[i].label ? fields[i].label : "", msg);
                        active = i;
                        ok = false;
                        break;
                    }
                }
            }
            if (!ok) continue;

            for (size_t i = 0; i < nfields; ++i) {
                buf_store(&bufs[i], fields[i].buf, fields[i].cap);
            }
            result = TUI_FORM_OK;
            break;
        }
        if (tui_key_printable(k) && valid_input_char(&fields[active], k.ch)) {
            if (bufs[active].len + 1 < FORM_MAX_CHARS) {
                bufs[active].text[bufs[active].len++] = k.ch;
                bufs[active].text[bufs[active].len] = L'\0';
            }
            error[0] = '\0';
            continue;
        }
    }

    curs_set(0);
    free(bufs);
    return result;
}
