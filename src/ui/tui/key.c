#include "ui/tui/key.h"

#include <curses.h>

tui_key tui_read_key(void) {
    tui_key k = { TUI_KEY_NONE, 0 };

    wint_t wc = 0;
    int rc = wget_wch(stdscr, &wc);

    if (rc == ERR) {
        /* 没有设置超时的情况下，ERR 只可能是 EOF 或读错误；
         * 当作退出信号处理，避免组件在非交互终端里忙等。 */
        k.type = TUI_KEY_EOF;
        return k;
    }

    if (rc == KEY_CODE_YES) {
        switch (wc) {
        case KEY_UP:     k.type = TUI_KEY_UP;     break;
        case KEY_DOWN:   k.type = TUI_KEY_DOWN;   break;
        case KEY_LEFT:   k.type = TUI_KEY_LEFT;   break;
        case KEY_RIGHT:  k.type = TUI_KEY_RIGHT;  break;
        case KEY_HOME:   k.type = TUI_KEY_HOME;   break;
        case KEY_END:    k.type = TUI_KEY_END;    break;
        case KEY_PPAGE:  k.type = TUI_KEY_PGUP;   break;
        case KEY_NPAGE:  k.type = TUI_KEY_PGDN;   break;
        case KEY_BACKSPACE: k.type = TUI_KEY_BACKSPACE; break;
        case KEY_DC:     k.type = TUI_KEY_DELETE; break;
        case KEY_BTAB:   k.type = TUI_KEY_BTAB;   break;
        case KEY_RESIZE: k.type = TUI_KEY_RESIZE; break;
        case KEY_F(1):   k.type = TUI_KEY_F1;     break;
        default:         k.type = TUI_KEY_NONE;   break;
        }
        k.ch = (wchar_t)wc;
        return k;
    }

    /* rc == OK：普通字符 */
    switch (wc) {
    case L'\n':
    case L'\r':
    case KEY_ENTER:
        k.type = TUI_KEY_ENTER;
        break;
    case 27:
        k.type = TUI_KEY_ESC;
        break;
    case L'\t':
        k.type = TUI_KEY_TAB;
        break;
    case 127:
    case 8:
        k.type = TUI_KEY_BACKSPACE;
        break;
    default:
        k.type = TUI_KEY_CHAR;
        k.ch = (wchar_t)wc;
        break;
    }
    return k;
}

bool tui_is_enter(tui_key k) {
    return k.type == TUI_KEY_ENTER;
}

bool tui_is_cancel(tui_key k) {
    return k.type == TUI_KEY_ESC || k.type == TUI_KEY_EOF ||
           tui_is_char(k, L'q') || tui_is_char(k, L'Q');
}

bool tui_is_prev(tui_key k) {
    return k.type == TUI_KEY_UP || tui_is_char(k, L'k') || tui_is_char(k, L'w');
}

bool tui_is_next(tui_key k) {
    return k.type == TUI_KEY_DOWN || tui_is_char(k, L'j') || tui_is_char(k, L's');
}

bool tui_is_prev_page(tui_key k) {
    return k.type == TUI_KEY_LEFT || k.type == TUI_KEY_PGUP ||
           tui_is_char(k, L'h') || tui_is_char(k, L'a');
}

bool tui_is_next_page(tui_key k) {
    return k.type == TUI_KEY_RIGHT || k.type == TUI_KEY_PGDN ||
           tui_is_char(k, L'l') || tui_is_char(k, L'd');
}

bool tui_is_backspace(tui_key k) {
    return k.type == TUI_KEY_BACKSPACE;
}

bool tui_is_char(tui_key k, wchar_t c) {
    return k.type == TUI_KEY_CHAR && k.ch == c;
}

bool tui_key_printable(tui_key k) {
    return k.type == TUI_KEY_CHAR && wcwidth(k.ch) >= 0;
}
