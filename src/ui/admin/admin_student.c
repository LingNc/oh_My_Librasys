#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "DataBase/DataBase.h"
#include "models/Student.h"
#include "function.h"
#include "ui/admin/admin_menu.h"
#include "ui/components/page.h"
#include "models/Manager.h"

extern dataBase studentDb;

void admin_student_preInfo(void *arg){
    admin_preInfo(arg);
    printf("查看学生列表功能  学生总数:%zu\n",studentDb->size(studentDb));
}

void admin_student_postInfo(void *arg){
    printf("\n输入/help重看帮助\n");
}

void import_students(void *arg){
    clear_screen();
    printf("按q键退出\n");
    printf("批量导入学生功能\n");
    char file_path[100];
    printf("请输入文件路径: ");
    if(!getaline(file_path,"q")){
        return;
    }
    bool res=load_students_from_file(file_path, studentDb);
    if(res) printf("批量导入学生成功\n");
    else printf("批量导入学生失败\n");
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_student_manual(void *arg){
    while(1){
        clear_screen();
        printf("增加学生功能\n");
        size_t id;
        char idStr[MAX_INPUT];
        char name[50],class[50],department[50];
        printf("请输入学生ID: ");
        if(!getaline(idStr,"q")){
            return;
        }
        id=(size_t)atoll(idStr);
        printf("请输入姓名: ");
        if(!getaline(name,"q")){
            return;
        }
        printf("请输入班级: ");
        if(!getaline(class,"q")){
            return;
        }
        printf("请输入学院: ");
        if(!getaline(department,"q")){
            return;
        }
        student s=new_student();
        load_student(s,0,id,name,class,department,0,"","");
        studentDb->add(studentDb,s);
        studentDb->save(studentDb);
        printf("增加学生成功\n");
        printf("是否继续添加? (y/n)\n");
        // getch();
        // 清空输入缓冲区
        char a[MAX_INPUT];
        if(!getaline(a,"qny")){
            if(a[0]!='y') return;
            else printf("继续添加\n");
        }
        if(strlen(a)!=1){
            printf("无效输入\n");
            getch();
            return;
        }
        getch(); // 等待用户按键
        clear_screen();
    }
}

void add_student(void *arg) {
    const wchar_t *choices[] = {
        L"1. 手动输入",
        L"2. 批量导入",
        L"3. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_preInfo, // preInfo
        add_student_manual,
        import_students,
        NULL  // postInfo
    };
    void *args[] = { arg, NULL, NULL, NULL };
    menu(n_choices, choices, funcs, args);
}

void edit_student(void *arg) {
    student s = (student)arg;

    if (s) {
        clear_screen();
        printf("修改学生功能\n");
        char name[50], class[50], department[50];

        printf("当前姓名: %s\n", s->name->c_str(s->name));
        printf("请输入新的姓名: ");
        if (!getaline(name, "q")) {
            return;
        }

        printf("\n当前班级: %s\n", s->class->c_str(s->class));
        printf("请输入新的班级: ");
        if (!getaline(class, "q")) {
            return;
        }

        printf("\n当前学院: %s\n", s->department->c_str(s->department));
        printf("请输入新的学院: ");
        if (!getaline(department, "q")) {
            return;
        }
        s->name->assign_cstr(s->name,name);
        s->class->assign_cstr(s->class,class);
        s->department->assign_cstr(s->department,department);
        studentDb->change(studentDb, s->id, s);
        printf("\n修改学生成功\n");
    } else {
        printf("\n学生不存在\n");
    }
    getch();
}

void delete_student(void *arg) {
    student s = (student)arg;
    if (s) {
        studentDb->rm(studentDb, s->id);
        studentDb->save(studentDb);
        printf("删除学生成功\n");
    } else {
        printf("学生不存在\n");
    }
    getchar();
}

void admin_student_manu(void *arg) {
    struct{
        manager m;
        student s;
    }*args=arg;
    student s=args->s;
    manager m=args->m;
    const wchar_t *choices[]={
        L"1. 修改学生",
        L"2. 删除学生",
        L"3. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *)={
        admin_preInfo,
        edit_student,
        delete_student,
        NULL
    };
    void *args_ptr[] = { m,s, s, NULL };
    menu(n_choices, choices, funcs, args_ptr);
}

void view_student_list(void *arg) {
    void (*funcs[])(void *)={
        admin_student_preInfo,
        admin_student_manu,
        admin_student_postInfo
    };
    bool show=false;
    void *args[]={ arg,NULL,arg,
        &show // 是否显示已借书籍
    };
    page(studentDb, 10, funcs, args);
}