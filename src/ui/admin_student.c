#include <stdio.h>
#include <wchar.h>
#include "ui/menu.h"
#include "ui/func.h"
#include "DataBase/DataBase.h"
#include "models/Student.h"
#include "function.h"
#include "ui/admin_menu.h"
#include "ui/page.h"

extern dataBase studentDb;

void import_students() {
    clear_screen();
    printf("按q键退出\n");
    printf("批量导入学生功能\n");
    char file_path[100];
    printf("请输入文件路径: ");
    scanf("%s", file_path);
    if (strcmp(file_path, "q") == 0)
        return;

    load_students_from_file(file_path, studentDb);
    printf("批量导入学生成功\n");
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_student_manual(void *arg) {
    clear_screen();
    printf("增加学生功能\n");
    while (1) {
        size_t id;
        char name[50], class[50], department[50];
        int borrowedCount;
        char borrowedDate[20], returnDate[20];

        printf("请输入学生ID: ");
        scanf("%zu", &id);
        printf("请输入姓名: ");
        scanf("%s", name);
        printf("请输入班级: ");
        scanf("%s", class);
        printf("请输入学院: ");
        scanf("%s", department);
        printf("请输入借阅数量: ");
        scanf("%d", &borrowedCount);
        printf("请输入借阅日期: ");
        scanf("%s", borrowedDate);
        printf("请输入归还日期: ");
        scanf("%s", returnDate);

        student s = new_student();
        load_student(s, id, name, class, department, borrowedCount, borrowedDate, returnDate);
        studentDb->add(studentDb, s);
        studentDb->save(studentDb);
        printf("增加学生成功\n");
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
    const char *info = "增加学生功能，请选择一个选项：";
    void *args[] = { (void *)info, NULL, NULL, NULL };
    menu(n_choices, choices, funcs, args);
}

void delete_student(void *arg) {
    clear_screen();
    printf("删除学生功能\n");
    size_t student_id;
    printf("按q退出\n");
    printf("请输入学生ID: ");
    scanf("%zu", &student_id);

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
    printf("\n按任意键返回，按'q'退出\n");
}

void display_student_list(void *arg) {
    vector students = (vector)arg;
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
    void (*funcs[])(void *) = { admin_student_preInfo, display_student_list, admin_student_postInfo };
    void *args[] = { arg };
    page(studentDb, 10, funcs, args);
}