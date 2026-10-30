#include "ui/tui/theme.h"

#include <curses.h>

static int g_color = 0;
static int g_gray = 0;

/* 256 色灰阶：232 近黑 → 255 近白 */
enum {
    GRAY_INK     = 233,   /* 最深 */
    GRAY_DEEP    = 236,
    GRAY_BODY    = 240,
    GRAY_DIM     = 243,
    GRAY_MID     = 246,
    GRAY_LIGHT   = 250,
    GRAY_PAPER   = 252,
    GRAY_BRIGHT  = 255
};

void theme_init(void) {
    g_color = 0;
    g_gray = 0;

    if (!has_colors()) return;

    start_color();
    use_default_colors();
    g_color = 1;
    g_gray = (COLORS >= 256);

    if (g_gray) {
        init_pair(UI_PAIR_NORMAL, GRAY_LIGHT,  -1);
        init_pair(UI_PAIR_TITLE,  GRAY_INK,    GRAY_MID);
        init_pair(UI_PAIR_HILITE, GRAY_INK,    GRAY_PAPER);
        init_pair(UI_PAIR_ERROR,  GRAY_BRIGHT, GRAY_DEEP);
        init_pair(UI_PAIR_HINT,   GRAY_DIM,    -1);
        init_pair(UI_PAIR_OK,     GRAY_BRIGHT, -1);
        init_pair(UI_PAIR_ART,    GRAY_MID,    -1);
        init_pair(UI_PAIR_BORDER, GRAY_BODY,   -1);
        return;
    }

    /* 8/16 色：用白/黑模仿灰阶，靠属性区分层次 */
    init_pair(UI_PAIR_NORMAL, COLOR_WHITE, -1);
    init_pair(UI_PAIR_TITLE,  COLOR_BLACK, COLOR_WHITE);
    init_pair(UI_PAIR_HILITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(UI_PAIR_ERROR,  COLOR_WHITE, COLOR_BLACK);
    init_pair(UI_PAIR_HINT,   COLOR_WHITE, -1);
    init_pair(UI_PAIR_OK,     COLOR_WHITE, -1);
    init_pair(UI_PAIR_ART,    COLOR_WHITE, -1);
    init_pair(UI_PAIR_BORDER, COLOR_WHITE, -1);
}

int theme_has_color(void) {
    return g_color;
}

int theme_has_gray_ramp(void) {
    return g_gray;
}

int theme_attr(ui_pair pair) {
    int base = A_NORMAL;

    if (g_color && pair > UI_PAIR_NORMAL) {
        base = COLOR_PAIR(pair);
    }

    switch (pair) {
    case UI_PAIR_TITLE:
        return base | A_BOLD;
    case UI_PAIR_HILITE:
        return base | (g_color ? A_NORMAL : A_REVERSE);
    case UI_PAIR_ERROR:
        return base | A_BOLD;
    case UI_PAIR_HINT:
        return base | (g_color ? A_NORMAL : A_DIM);
    case UI_PAIR_OK:
        return base | A_BOLD;
    case UI_PAIR_ART:
        return base | A_BOLD;
    case UI_PAIR_BORDER:
        return base | (g_color ? A_NORMAL : A_DIM);
    default:
        return base;
    }
}
