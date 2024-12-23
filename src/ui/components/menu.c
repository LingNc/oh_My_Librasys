#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "ui/components/menu.h"
#include "ui/components/list.h"
#include "ui/components/func.h"
#include "DataBase/DataBase.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "ui/admin_menu.h"
#include "ui/student_menu.h"
#include "ui/command.h"

extern dataBase studentDb, managerDb;

void display_menu(int highlight, const wchar_t **choices, int n_choices) {
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
    case 'q':
        *choice = n_choices; // 退出
        break;
    default:
        if (ch >= '0' && ch <= '9') {
            *highlight = ((ch - '0') - 1) % n_choices;
            direct_jump = true;
        }
        break;
    }
    return direct_jump || ch == '\n' || ch == 'q';
}

void menu(int n_choices, const wchar_t **choices, void (**funcs)(void *), void **arg) {
    int highlight = 0;
    int choice = -1;

    while (1) {
        clear_screen();
        // preInfo
        if(funcs[0]){
            if(arg){
                if(arg[0]) funcs[0](arg[0]);
                else funcs[0](NULL);
            }
            else funcs[0](NULL);
        }
        display_menu(highlight, choices, n_choices);
        // postInfo
        if (funcs[n_choices]){
            if(arg){
                if(arg[n_choices]) funcs[n_choices](arg[n_choices]);
                else funcs[n_choices](NULL);
            }
            else funcs[n_choices](NULL);
        }
        bool res = handle_menu_input(&highlight, n_choices, &choice);
        if (choice != -1 && res) {
            for (int i = 1; i < n_choices; i++) {
                if (choice == i) {
                    // 执行对应的函数
                    if(arg){
                        if(arg[i]) funcs[i](arg[i]);
                        else funcs[i](NULL);
                    }
                    else funcs[i](NULL);
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
        }
    }
}