#include "ui/components/func.h"
#include "ui/admin/admin_menu.h"
#include "ui/student/student_menu.h"
#include "DataBase/DataBase.h"
#include "models/Manager.h"
#include "models/Student.h"

extern dataBase managerDb,studentDb;

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
    char idStr[MAX_INPUT];
    if(!getaline(idStr,"q")){
        return;
    }
    id=(size_t)atoi(idStr);
    // void *args[] = { &id };

    if (is_admin) {
        manager m = managerDb->find_key(managerDb, id);
        if (m) {
            admin_menu(m);
        } else {
            printf("管理员ID不存在请重新输入\n");
            getch();
            // login(arg);
        }
    } else {
        student s = studentDb->find_key(studentDb, id);
        if (s) {
            student_menu(s);
        } else {
            printf("学号输入有误，请重新输入\n");
            getch();
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