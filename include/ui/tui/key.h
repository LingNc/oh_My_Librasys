#ifndef _ui_key_h
#define _ui_key_h

#include <stdbool.h>
#include <wchar.h>

/* 键抽象：把 ncurses 的原始输入统一成与库无关的语义键 */

typedef enum {
    TUI_KEY_NONE = 0,
    TUI_KEY_UP, TUI_KEY_DOWN, TUI_KEY_LEFT, TUI_KEY_RIGHT,
    TUI_KEY_ENTER, TUI_KEY_ESC, TUI_KEY_TAB, TUI_KEY_BTAB,
    TUI_KEY_BACKSPACE, TUI_KEY_DELETE,
    TUI_KEY_PGUP, TUI_KEY_PGDN, TUI_KEY_HOME, TUI_KEY_END,
    TUI_KEY_RESIZE, TUI_KEY_F1,
    TUI_KEY_EOF,          /* 输入结束（非交互终端 / 管道） */
    TUI_KEY_CHAR          /* 可打印字符，值在 ch 中 */
} tui_key_type;

typedef struct {
    tui_key_type type;
    wchar_t ch;
} tui_key;

/* 读取一个键（唯一接触 wget_wch 的地方；阻塞） */
tui_key tui_read_key(void);

/* 语义判定（同时接受方向键与 vim 风格字符键） */
bool tui_is_enter(tui_key k);
bool tui_is_cancel(tui_key k);      /* Esc / q / Q / EOF */
bool tui_is_prev(tui_key k);        /* ↑ / k / w */
bool tui_is_next(tui_key k);        /* ↓ / j / s */
bool tui_is_prev_page(tui_key k);   /* ← / PgUp / h / a */
bool tui_is_next_page(tui_key k);   /* → / PgDn / l / d */
bool tui_is_backspace(tui_key k);
bool tui_is_char(tui_key k, wchar_t c);

/* 可打印字符（非控制字符） */
bool tui_key_printable(tui_key k);

#endif // _ui_key_h
