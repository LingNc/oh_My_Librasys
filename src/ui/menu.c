#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "ui/menu.h"
#include "ui/list.h"
#include "ui/func.h"
#include "DataBase/DataBase.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "ui/admin_menu.h"
#include "ui/student_menu.h"
#include "ui/command.h"

extern dataBase studentDb, managerDb;

void display_menu(int highlight, const wchar_t **choices, int n_choices, const char *info) {
    clear_screen();
    if (info) {
        printf("%s\n", info);
    }
    display_wchar(highlight, choices, n_choices);
}

bool handle_menu_input(int *highlight, int n_choices, int *choice) {
    char ch = getch();
    bool direct_jump = false;

    switch (ch) {
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
        if (ch >= '0' && ch <= '9') {
            *highlight = ((ch - '0') - 1) % n_choices;
            direct_jump = true;
        }
        break;
    }
    return direct_jump || ch == '\n';
}

void menu(const wchar_t **choices, void (**funcs)(void **), int n_choices, void **arg, const char *info) {
    int highlight = 0;
    int choice = -1;

    while (1) {
        display_menu(highlight, choices, n_choices, info);
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
                break;
            }
            if (choice > n_choices) {
                printf("无效选择，请重新选择\n");
                getchar();
            }
            choice = -1; // 重置选择
        } else if (res && choice == -1) {
            if (getch() == '/') {
                execute(NULL);
            }
        }
    }
}