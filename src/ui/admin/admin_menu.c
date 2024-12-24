#include <stdio.h>
#include <wchar.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "ui/admin/admin_book.h"
#include "ui/admin/admin_student.h"
#include "DataBase/DataBase.h"
#include "models/Manager.h"
#include "function.h"

extern dataBase managerDb;

void admin_preInfo(void *arg){
    manager m = (manager)arg;
    printf("当前账号: ID: %zu, 姓名: %s, 注册日期: %s, 注册人: %s\n管理员总数: %zu\n",
           m->id, m->name->c_str(m->name), m->registration_date->c_str(m->registration_date), m->registered_by->c_str(m->registered_by), managerDb->size(managerDb));
}

void admin_postInfo(void *arg) {
    printf("\n输入/help查看帮助\n");
}

void add_manager(void *arg){
    manager m=(manager)arg;
    while(1){
        clear_screen();
        printf("增加管理员功能\n");
        size_t id;
        char name[50],idStr[MAX_INPUT];

        printf("请输入管理员ID: ");
        if (!getaline(idStr, "q")) {
            return;
        }
        id = (size_t)atoll(idStr);

        printf("请输入姓名: ");
        if (!getaline(name, "q")) {
            return;
        }

        // 获取当前时间
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char date[20];
        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", t);

        manager newM = new_manager();
        load_manager(newM, id, name, date, m->name->c_str(m->name));
        managerDb->add(managerDb, newM);
        managerDb->save(managerDb);
        printf("增加管理员成功\n");
        printf("是否继续添加? (y/n)\n");
        // 清空输入缓冲区
        char a[MAX_INPUT];
        if (!getaline(a, "qn")) {
            return;
        }
        if (a[0] != 'y') break;
        clear_screen();
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void edit_manager(void *arg) {
    manager m = (manager)arg;

    if (m) {
        clear_screen();
        printf("修改管理员功能\n");
        char name[50];

        printf("当前姓名: %s\n", m->name->c_str(m->name));
        printf("请输入新的姓名: ");
        if (!getaline(name, "q")) {
            return;
        }

        m->name->assign_cstr(m->name, name);
        managerDb->change(managerDb, m->id, m);
        printf("\n修改管理员成功\n");
    } else {
        printf("\n管理员不存在\n");
    }
    getch();
}

void delete_manager(void *arg) {
    manager m = (manager)arg;

    if (m) {
        managerDb->rm(managerDb, m->id);
        managerDb->save(managerDb);
        printf("删除管理员成功\n");
    } else {
        printf("管理员不存在\n");
    }
    getchar();
}

void manager_menu(void *arg) {
    struct {
        manager m;
    } *args = arg;
    manager m = args->m;
    const wchar_t *choices[] = {
        L"1. 修改管理员",
        L"2. 删除管理员",
        L"3. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_preInfo,
        edit_manager,
        delete_manager,
        NULL
    };
    void *args_ptr[] = { m, m, m, NULL };
    menu(n_choices, choices, funcs, args_ptr);
}

void change_password(void *arg) {
    clear_screen();
    printf("修改密码功能\n");
    // 实现修改密码的逻辑
    getch(); // 等待用户按键
    clear_screen();
}

void display_manager_list(void *arg) {
    vector managers = (vector)arg;
    if (managers->size(managers) == 0) {
        printf("没有管理员信息\n");
        return;
    }
    printf("%-10s %-20s %-20s %-20s\n", "管理员ID", "姓名", "注册日期", "注册人");
    for (size_t i = 0; i < managers->size(managers); i++) {
        manager m = managers->at(managers, i);
        if (m) {
            printf("%-10zu %-20s %-20s %-20s\n",
                   m->id, m->name->c_str(m->name), m->registration_date->c_str(m->registration_date), m->registered_by->c_str(m->registered_by));
        }
    }
}

void view_manager_list(void *arg) {
    void (*funcs[])(void *) = {
        admin_preInfo,
        manager_menu,
        admin_postInfo
    };
    bool show = false;
    void *args[] = { arg, NULL, arg, &show };
    page(managerDb, 10, funcs, args);
}

void admin_menu(void *arg) {
    manager m = (manager)arg;
    const wchar_t *choices[] = {
        L"1. 学生管理",
        L"2. 新建学生",
        L"3. 图书管理",
        L"4. 增加图书",
        L"5. 管理员管理",
        L"6. 增加管理员",
        L"7. 修改密码",
        L"8. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_preInfo, // preInfo
        view_student_list,
        add_student,
        view_book_list,
        add_book,
        view_manager_list,
        add_manager,
        change_password,
        NULL  // postInfo
    };
    void *args[] = { m, m, m, m, m, m, m, m, m };
    menu(n_choices, choices, funcs, args);
}