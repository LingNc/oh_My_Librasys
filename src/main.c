#include "DataBase/DataBase.h"
#include "models/uiBook.h"
#include "models/Book.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "function.h"
#include "ui/func.h"
#include "ui/menu.h"
#include "ui/admin_menu.h"
#include "ui/student_menu.h"
#include "ui/page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "time.h"
#include "ui/command.h"
#include "ui/admin_book.h"
#include "ui/admin_student.h"

dataBase bookDb, studentDb, borrowDb, managerDb;

void login(void *arg){
    clear_screen();
    bool is_admin = *(bool *)arg;
    size_t id;
    if(is_admin){
        printf("管理员登录\n");
        printf("账号: ");    }
    else{
        printf("学生登录\n");
        printf("学号: ");
    }
    scanf("%zu", &id);
    getchar(); // 读走回车
    void *args[] = { &id };

    if (is_admin) {
        manager m = managerDb->find_key(managerDb, id);
        if (m) {
            admin_menu(args);
        } else {
            printf("管理员ID不存在，请重新输入\n");
            getchar();
            // login(arg);
        }
    } else {
        student s = studentDb->find_key(studentDb, id);
        if (s) {
            student_menu(args);
        } else {
            printf("学号输入有误，请重新输入\n");
            getchar();
            // login(arg);
        }
    }
}

void preInfo(void *arg) {
    printf("欢迎使用图书管理系统\n");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("当前时间: %02d:%02d\n", t->tm_hour, t->tm_min);
    printf("请选择一个选项：\n");
}

void main_menu() {
    const wchar_t *choices[] = {
        L"1. 学生入口",
        L"2. 管理员入口",
        L"3. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        preInfo, // preInfo
        login,
        login,
        NULL  // postInfo
    };
    bool is_admin[] = { false, true };
    void *args[] = { NULL, &is_admin[0], &is_admin[1], NULL };
    menu(n_choices, choices, funcs, args);
}

int main() {
    setlocale(LC_ALL, "");

    bookDb = database("db/book", Book);
    studentDb = database("db/student", Student);
    borrowDb = database("db/borrow_records", String);
    managerDb = database("db/manager", Manager);

    // 初始化自动补全
    init_autocomplete();

    // 初始化根管理用户
    manager root = new_manager();
    load_manager(root, 1, "root", "2024-01-01", "system");
    managerDb->add(managerDb, root);
    managerDb->save(managerDb);
    free_manager(root);

    main_menu();

    return 0;
}
