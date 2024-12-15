#ifndef TERMINAL_H
#define TERMINAL_H

#include <ncursesw/ncurses.h>

typedef struct {
    int width;
    int height;
} TerminalSize;

extern TerminalSize terminal;

void update_terminal_size();

#endif