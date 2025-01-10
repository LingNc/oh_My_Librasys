#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "ui/components/page.h"
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Student.h"
#include "function.h"
#include "ui/student/student_borrow.h"
#include "ui/student/student_return.h"

#define DEFAULT_PAGE_SIZE 10

extern dataBase studentDb,borrowDb,bookDb;

void student_preInfo(void *arg){
    student s=(student)arg;
    printf("学生信息: 学号: %zu, 姓名: %s, 班级: %s, 学院: %s, 借阅数量: %d\n",
        s->studentID,s->name->c_str(s->name),s->class->c_str(s->class),s->department->c_str(s->department),s->borrowedCount);
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    printf("当前时间: %02d:%02d\n",t->tm_hour,t->tm_min);
    printf("请选择一个操作:\n");
}

void student_postInfo(void *arg){
    printf("\n输入/help查看帮助\n");
}

// void view_borrow_info(void **arg){
//     size_t student_id=*(size_t *)arg[0];
//     clear_screen();
//     printf("查看借阅信息功能\n");

//     student s=studentDb->find_key(studentDb,student_id);
//     if(s){
//         printf("学生ID: %zu\n",s->id);
//         printf("姓名: %s\n",s->name->c_str(s->name));
//         printf("班级: %s\n",s->class->c_str(s->class));
//         printf("学院: %s\n",s->department->c_str(s->department));
//         printf("借阅数量: %d\n",s->borrowedCount);
//         vector borrow_records=load_borrow_records(borrowDb,s->id);
//         for(size_t i=0; i<borrow_records->size(borrow_records); ++i){
//             string record=(string)borrow_records->at(borrow_records,i);
//             size_t book_id;
//             time_t timestamp;
//             memcpy(&book_id,record->c_str(record),sizeof(size_t));
//             memcpy(&timestamp,record->c_str(record)+sizeof(size_t),sizeof(time_t));
//             book b=bookDb->find_key(bookDb,book_id);
//             if(b){
//                 printf("书籍ID: %zu, 书名: %s, 借出时间: %s",b->id,b->name->c_str(b->name),ctime(&timestamp));
//             }
//         }
//         borrow_records->free(borrow_records);
//     }
//     else{
//         printf("当前学生尚未借书，暂未借阅信息。\n");
//     }
//     getchar();  // 等待用户按键
//     clear_screen();
// }

void student_menu(void *arg){
    student s=(student)arg;
    if(!s){
        printf("学生不存在\n");
        return;
    }

    const wchar_t *choices[]={
        L"1. 借书",
        L"2. 还书",
        L"3. 退出"
    };
    int n_choices=sizeof(choices)/sizeof(choices[0]);
    void (*funcs[])(void *)={
        student_preInfo, // preInfo
        borrow_book_menu,
        return_book_menu,
        student_postInfo  // postInfo
    };
    void *args[]={ s,s,s,NULL };

    menu(n_choices,choices,funcs,args);
}