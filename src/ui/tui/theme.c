#include "ui/tui/theme.h"

#include <curses.h>

static int g_color = 0;

void theme_init(void) {
    if (!has_colors()) {
        g_color = 0;
        return;
    }
    start_color();
    use_default_colors();

    init_pair(UI_PAIR_NORMAL,   -1,             -1);
    init_pair(UI_PAIR_TITLE,    COLOR_BLACK,    COLOR_CYAN);
    init_pair(UI_PAIR_HILITE,   COLOR_BLACK,    COLOR_CYAN);
    init_pair(UI_PAIR_ERROR,    COLOR_RED,      -1);
    init_pair(UI_PAIR_HINT,     COLOR_BLUE,     -1);
    init_pair(UI_PAIR_OK,       COLOR_GREEN,    -1);

    g_color = 1;
}

int theme_has_color(void) {
    return g_color;
}

int theme_attr(ui_pair pair) {
    if (g_color && pair > UI_PAIR_NORMAL) {
        return COLOR_PAIR(pair);
    }
    switch (pair) {
    case UI_PAIR_TITLE:
        return A_BOLD;
    case UI_PAIR_HILITE:
        return A_REVERSE;
    case UI_PAIR_ERROR:
        return A_BOLD;
    case UI_PAIR_HINT:
        return A_DIM;
    case UI_PAIR_OK:
        return A_BOLD;
    default:
        return A_NORMAL;
    }
}
