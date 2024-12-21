
#include <stdio.h>
#include "ui/line.h"

void display_line(int highlight, const wchar_t **lines, int n_lines) {
    for (int i = 0; i < n_lines; ++i) {
        if (i == highlight) {
            printf("> %ls\n", lines[i]);
        } else {
            printf("  %ls\n", lines[i]);
        }
    }
}