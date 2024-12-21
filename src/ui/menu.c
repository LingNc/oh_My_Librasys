#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "ui/menu.h"
#include "ui/line.h"
#include "DataBase/DataBase.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "ui/admin_menu.h"
#include "ui/student_menu.h"

extern dataBase studentDb, managerDb;

void clear_screen() {
    printf("\033[H\033[J");
}

void display_menu(int highlight, const wchar_t **choices, int n_choices) {
    clear_screen();
    display_line(highlight, choices, n_choices);
}

bool handle_menu_input(int *highlight, int n_choices, int *choice) {
    char line[MAX_INPUT] = { 0 };
    fgets(line, MAX_INPUT, stdin);
    bool direct_jump = false;
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
                *highlight = ((line[i] - '0') - 1) % n_choices;
                direct_jump = true;
            }
            break;
        }
    }
    return direct_jump || (strlen(line) == 1 && line[0] == '\n');
}

void menu(const wchar_t **choices, void (**funcs)(void **), int n_choices, void **arg) {
    int highlight = 0;
    int choice = -1;
    while (1) {
        display_menu(highlight, choices, n_choices);
        bool res = handle_menu_input(&highlight, n_choices, &choice);
        if (choice != -1 && res) {
            for (int i = 0; i < n_choices - 1; i++) {
                if (choice - 1 == i) {
                    // 执行对应的函数
                    funcs[i](&arg[i]);
                }
            }
            // 最后一个是退出命令
            if (choice == n_choices) {
                printf("退出\n");
                return;
            }
            if (choice > n_choices) {
                printf("无效选择，请重新选择\n");
                getchar();
            }
            choice = -1; // 重置选择
        }
    }
}