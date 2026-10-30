#include "ui/tui/components/prompt.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "ui/tui/key.h"
#include "ui/tui/theme.h"
#include "ui/tui/width.h"
#include "ui/tui/window.h"

#define PROMPT_MAX_CHARS 256

static void load_initial(wchar_t *text, size_t *len, const char *initial) {
    *len = 0;
    text[0] = L'\0';
    if (!initial) return;

    wchar_t *w = ui_wcs_from_utf8(initial);
    if (!w) return;

    size_t n = wcslen(w);
    if (n >= PROMPT_MAX_CHARS) n = PROMPT_MAX_CHARS - 1;
    wmemcpy(text, w, n);
    text[n] = L'\0';
    *len = n;
    free(w);
}

tui_prompt_result tui_prompt(const char *title, const char *initial,
                             char *buf, size_t cap,
                             tui_prompt_complete_fn complete) {
    if (!buf || cap == 0) return TUI_PROMPT_CANCEL;

    wchar_t text[PROMPT_MAX_CHARS];
    size_t len = 0;
    load_initial(text, &len, initial);

    for (;;) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);

        int w = cols - 4;
        if (w < 20) w = cols;
        int x = (cols - w) / 2;
        int y = rows - 4;
        if (y < 0) y = 0;

        /* 清掉输入框区域，避免与下方内容重叠 */
        for (int i = 0; i < 3 && y + i < rows; ++i) {
            mvhline(y + i, x, ' ', w);
        }

        WINDOW *win = derwin(stdscr, 3, w, y, x);
        if (win) {
            box(win, 0, 0);
            if (title && title[0]) {
                wattron(win, theme_attr(UI_PAIR_TITLE));
                mvwaddstr(win, 0, 2, " ");
                waddstr(win, title);
                waddstr(win, " ");
                wattroff(win, theme_attr(UI_PAIR_TITLE));
            }

            int width = w - 4;
            if (width < 1) width = 1;
            ui_puts_w(y + 1, x + 2, width, text, theme_attr(UI_PAIR_NORMAL));

            int cursor = x + 2 + (int)ui_wcswidth(text);
            int limit = x + w - 3;
            if (cursor > limit) cursor = limit;
            wmove(stdscr, y + 1, cursor);
            curs_set(1);

            wnoutrefresh(win);
            delwin(win);
        }
        wnoutrefresh(stdscr);
        doupdate();

        tui_key k = tui_read_key();

        if (k.type == TUI_KEY_RESIZE) continue;
        if (k.type == TUI_KEY_ESC) { curs_set(0); return TUI_PROMPT_CANCEL; }
        if (tui_is_enter(k)) {
            curs_set(0);
            char *u = ui_utf8_from_wcs(text);
            ui_utf8_copy(buf, cap, u ? u : "");
            free(u);
            return TUI_PROMPT_OK;
        }
        if (tui_is_backspace(k)) {
            if (len > 0) text[--len] = L'\0';
            continue;
        }
        if (k.type == TUI_KEY_TAB) {
            if (!complete) continue;
            char *u = ui_utf8_from_wcs(text);
            char suggestion[512];
            suggestion[0] = '\0';
            complete(u ? u : "", suggestion, sizeof(suggestion));
            free(u);

            if (suggestion[0]) {
                load_initial(text, &len, suggestion);
            }
            continue;
        }
        if (tui_key_printable(k)) {
            if (len + 1 < PROMPT_MAX_CHARS) {
                text[len++] = k.ch;
                text[len] = L'\0';
            }
        }
    }
}
