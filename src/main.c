#include "DataBase/DataBase.h"
#include "models/uiBook.h"
#include "models/Book.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "function.h"
#include "ui/menu.h"
#include "ui/student_menu.h"
#include "ui/admin_menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "time.h"

dataBase bookDb, studentDb, borrowDb, managerDb;

void main_menu() {
    const wchar_t *choices[] = {
        L"1. 学生入口",
        L"2. 管理员入口",
        L"3. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        student_menu,
        book_management_menu
    };
    menu(choices, funcs, n_choices, NULL);
}

int main() {
    setlocale(LC_ALL, "");

    bookDb = database("db/book", Book);
    studentDb = database("db/student", Student);
    borrowDb = database("db/borrow_records", String);
    managerDb = database("db/manager", Manager);

    // 初始化根管理用户
    manager root = new_manager();
    load_manager(root, 1, "root", "2024-01-01", "system");
    managerDb->add(managerDb, root);
    managerDb->save(managerDb);
    free_manager(root);

    main_menu();

    return 0;
}
