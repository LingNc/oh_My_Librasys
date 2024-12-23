#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "DataBase/DataBase.h"
#include "models/Student.h"
#include "function.h"
#include "ui/admin_menu.h"
#include "ui/components/page.h"

extern dataBase studentDb;

void import_students(void* arg) {
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

void delete_student(void *arg) {
    clear_screen();
    printf("删除学生功能\n");
    size_t student_id;
    char id[MAX_INPUT];
    printf("按q退出\n");
    printf("请输入学生ID: ");
    if(!getaline(id,"q")){
        return;
    }
    student_id=(size_t)atoi(id);
    student s = studentDb->find_key(studentDb, student_id);
    if (s) {
        studentDb->rm(studentDb, student_id);
        studentDb->save(studentDb);
        printf("删除学生成功\n");
    } else {
        printf("学生不存在\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void admin_student_preInfo(void *arg) {
    printf("查看学生列表功能\n");
}

void admin_student_postInfo(void *arg) {
    printf("\n输入/help重看帮助\n");
}

void display_student_list(void *arg) {
    vector students = (vector)arg;
    if (students->size(students) == 0) {
        printf("没有学生信息\n");
        return;
    }
    printf("%-10s %-20s %-12s %-12s %-10s\n", "学生ID", "姓名", "班级", "学院", "借阅数量");
    for (size_t i = 0; i < students->size(students); i++) {
        student s = students->at(students, i);
        if (s) {
            printf("%-10zu %-20s %-12.12s %-12.12s %-10d\n",
                   s->id, s->name->c_str(s->name), s->class->c_str(s->class), s->department->c_str(s->department), s->borrowedCount);
        }
    }
}

void view_student_list(void *arg) {
    void (*funcs[])(void *)={
        admin_student_preInfo,
        display_student_list,
        admin_student_postInfo
    };
    bool show=false;
    void *args[]={ arg,NULL,arg,
        &show // 是否显示已借书籍
    };
    page(studentDb, 10, funcs, args);
}