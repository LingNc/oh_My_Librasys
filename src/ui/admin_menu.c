#include <stdio.h>
#include <wchar.h>
#include "ui/menu.h"
#include "ui/func.h"
#include "ui/admin_book.h"
#include "ui/admin_student.h"
#include "DataBase/DataBase.h"
#include "models/Manager.h"
#include "function.h"

extern dataBase managerDb;

void admin_preInfo(void *arg) {
    const char *info = (const char *)arg;
    printf("%s\n", info);
}

void add_manager(void *arg) {
    clear_screen();
    printf("增加管理员功能\n");
    while (1) {
        size_t id;
        char name[50], date[20], role[50];

        printf("请输入管理员ID: ");
        scanf("%zu", &id);
        printf("请输入姓名: ");
        scanf("%s", name);
        printf("请输入创建日期: ");
        scanf("%s", date);
        printf("请输入角色: ");
        scanf("%s", role);

        manager m = new_manager();
        load_manager(m, id, name, date, role);
        managerDb->add(managerDb, m);
        managerDb->save(managerDb);
        printf("增加管理员成功\n");
        printf("是否继续添加? (y/n)\n");
        // 清空输入缓冲区
        getchar();
        char a;
        scanf("%s", &a);
        if (a == 'n')
            break;
        clear_screen();
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void delete_manager(void *arg) {
    clear_screen();
    printf("删除管理员功能\n");
    size_t manager_id;
    printf("按q退出\n");
    printf("请输入管理员ID: ");
    scanf("%zu", &manager_id);

    manager m = managerDb->find_key(managerDb, manager_id);
    if (m) {
        managerDb->rm(managerDb, manager_id);
        managerDb->save(managerDb);
        printf("删除管理员成功\n");
    } else {
        printf("管理员不存在\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void change_password(void *arg) {
    clear_screen();
    printf("修改密码功能\n");
    // 实现修改密码的逻辑
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void admin_menu(void **arg) {
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
        delete_manager,
        add_manager,
        change_password,
        NULL  // postInfo
    };
    const char *info = "管理员菜单，请选择一个选项：";
    void *args[] = { (void *)info, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    menu(n_choices, choices, funcs, args);
}
