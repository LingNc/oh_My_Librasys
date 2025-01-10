#include <stdio.h>
#include <wchar.h>
#include "ui/components/menu.h"
#include "ui/components/page.h"
#include "ui/components/func.h"
#include "ui/components/passwd.h"
#include "ui/admin/admin_book.h"
#include "ui/admin/admin_student.h"
#include "DataBase/DataBase.h"
#include "models/Manager.h"
#include "Tools/Hash.h"
#include "function.h"

extern dataBase managerDb,passwordDb;

void admin_preInfo(void *arg){
    manager m = (manager)arg;
    printf("当前账号: ID: %zu, 姓名: %s, 注册日期: %s, 注册人: %s\n管理员总数: %zu\n",
           m->id, m->name->c_str(m->name), m->registration_date->c_str(m->registration_date), m->registered_by->c_str(m->registered_by), managerDb->size(managerDb));
}

void admin_other_preInfo(void *arg){
    struct{
        manager me;
        manager it;
    }*args=arg;
    manager me=args->me;
    manager it=args->it;
    admin_preInfo(me);

    printf("选中账号: \nID: %zu, 姓名: %s, 注册日期: %s, 注册人: %s\n",
           it->id, it->name->c_str(it->name), it->registration_date->c_str(it->registration_date), it->registered_by->c_str(it->registered_by));
}

void admin_postInfo(void *arg) {
    printf("\n输入/help查看帮助\n");
}

bool admin_vertify_password(size_t id) {
    string stored_hash = passwordDb->find_key(passwordDb, id);
    if (!stored_hash) {
        return false;
    }
    string input_password = get_password("请验证您的密码: ");
    string input_hash=sha256(input_password);
    int res=strcmp(stored_hash->c_str(stored_hash),input_hash->c_str(input_hash));
    if(res==0){
        return true;
    }
    else{
        printf("密码验证失败\n");
        printf("按任意键继续\n");
        getch();
        return false;
    }
}

// 权限认证
bool admin_authenticate(void* arg){
    struct{
        manager me;
        manager it;
    }*args=arg;
    manager me=args->me;
    manager it=args->it;
    // root 账户绝对权限
    if(me->id!=1){
        // 验证管理员权限
        if(it->registered_by->cmp(it->registered_by,me->name)!=0){
            printf("权限不足,该管理员由 %s 管理\n",it->registered_by->c_str(it->registered_by));
            printf("按任意键继续\n");
            getch();
            return false;
        }
    }
    return true;
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

        if(managerDb->find_key(managerDb, id)){
            printf("管理员ID已存在，请重新输入\n");
            getch();
            continue;
        }
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

        // 设置密码
        bool password_set = false;
        for (int attempts = 0; attempts < 3; attempts++) {
            string password = get_password("请输入密码: ");
            string confirm_password = get_password("请确认密码: ");
            if (strcmp(password->c_str(password), confirm_password->c_str(confirm_password)) == 0) {
                string hashed_password = sha256(password);
                passwordDb->add_key(passwordDb, hashed_password, id);
                passwordDb->save(passwordDb);
                printf("增加管理员成功\n");
                password_set = true;
                break;
            } else {
                printf("两次输入的密码不一致，请重试\n");
            }
        }

        if (!password_set) {
            printf("密码设置失败，管理员添加失败\n");
            managerDb->rm(managerDb, id);
            managerDb->save(managerDb);
        }

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
    struct{
        manager me;
        manager it;
    } *args = arg;
    manager me=args->me;
    manager it=args->it;
    clear_screen();
    if (it) {
        printf("修改管理员功能\n");
        char name[50];

        // 鉴权
        if(!admin_authenticate(arg)) return;
        // 验证密码
        if(!admin_vertify_password(me->id)) return;

        printf("当前姓名: %s\n",it->name->c_str(it->name));
        printf("请输入新的姓名: ");
        if (!getaline(name, "q")) {
            return;
        }

        it->name->assign_cstr(it->name, name);
        managerDb->change(managerDb, it->id, it);
        printf("\n修改管理员成功\n");
    } else {
        printf("\n管理员不存在\n");
    }
    getch();
}

void delete_manager(void *arg) {
    struct{
        manager me;
        manager it;
    } *args=arg;
    manager me=args->me;
    manager it=args->it;
    clear_screen();
    if(it){
        // 鉴权
        if(!admin_authenticate(arg)) return;
        // 验证密码
        if(!admin_vertify_password(me->id)) return;
        managerDb->rm(managerDb,it->id);
        managerDb->save(managerDb);
        printf("删除管理员成功\n");
    } else {
        printf("管理员不存在\n");
    }
    printf("按任意键继续\n");
    getch();
}


void edit_password(void *arg){
    struct{
        manager me;
        manager it;
    } *args=arg;
    manager me=args->me;
    manager it=args->it;
    clear_screen();
    printf("修改密码功能\n");
    // 鉴权
    if(!admin_authenticate(arg)) return;
    // 验证密码
    if(!admin_vertify_password(me->id)) return;
    string new_password=get_password("请输入新密码: ");
    string confirm_password = get_password("请再次输入新密码: ");
    if (strcmp(new_password->c_str(new_password), confirm_password->c_str(confirm_password)) == 0) {
        string new_hash = sha256(new_password);
        passwordDb->change(passwordDb, it->id, new_hash);
        passwordDb->save(passwordDb);
        printf("密码修改成功\n");
    } else {
        printf("两次输入的密码不一致\n");
    }
    printf("按任意键继续\n");
    getch(); // 等待用户按键
}

void manager_menu(void *arg) {
    struct{
        manager me;
        manager it;
    } *args = arg;
    manager me=args->me;
    manager it=args->it;
    const wchar_t *choices[]={
        L"1. 修改信息",
        L"2. 修改密码",
        L"3. 删除管理员",
        L"4. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_other_preInfo,
        edit_manager,
        edit_password,
        delete_manager,
        NULL
    };

    void *args_ptr[]={ args,args,args,args,NULL };
    menu(n_choices, choices, funcs, args_ptr);
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
        edit_password,
        NULL  // postInfo
    };
    struct{
        manager m0;
        manager m1;
    }out_args={m,m};
    void *args[]={ m,m,m,m,m,m,m,&out_args,m };
    menu(n_choices, choices, funcs, args);
}
