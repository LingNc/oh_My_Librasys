#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "menu.h"

void display_menu(int highlight, const wchar_t **choices, int n_choices) {
    printf("\033[H\033[J"); // 清屏
    for (int i = 0; i < n_choices; ++i) {
        if (i == highlight) {
            printf("> \033[43;37m%ls\033[0m\n", choices[i]); // 设置背景色为黄色，字体为白色
        } else {
            printf("  %ls\n", choices[i]);
        }
    }
}

bool handle_menu_input(int *highlight, int n_choices, int *choice) {
    char line[100] = {0};
    fgets(line, 100, stdin);
    for (size_t i = 0; i < strlen(line); i++) {
        switch (line[i]) {
            case 'w':
                *highlight = (*highlight - 1 + n_choices) % n_choices; // 上移光标
                break;
            case 's':
                *highlight = (*highlight + 1) % n_choices; // 下移光标
                break;
            case '\n':
                *choice = *highlight + 1;
                break;
            default:
                if (line[i] >= '0' && line[i] <= '9') {
                    *highlight = (line[i] - '0') % n_choices;
                }
                break;
        }
    }
    return (line[0]=='\n'&&strlen(line)==1)?true:false;
}