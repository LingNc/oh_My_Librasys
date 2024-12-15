#include "terminal.h"

TerminalSize terminal;

void update_terminal_size() {
    getmaxyx(stdscr, terminal.height, terminal.width);
}