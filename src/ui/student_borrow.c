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
#include "ui/student_borrow.h"
#include "ui/student_return.h"

#define DEFAULT_PAGE_SIZE 10

extern dataBase studentDb,borrowDb,bookDb;

void display_book_details(void *arg){
    struct{
        student s;
        book b;
    }*args=arg;
    student s=args->s;
    book b=args->b;
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    printf("当前时间: %02d:%02d\n",t->tm_hour,t->tm_min);
    printf("学生信息: ID: %zu, 姓名: %s, 班级: %s, 学院: %s, 借阅数量: %d\n\n",
        s->id,s->name->c_str(s->name),s->class->c_str(s->class),s->department->c_str(s->department),s->borrowedCount);
    printf("书籍详情: \n书名: %s  作者: %s, 出版社: %s  出版日期: %s  状态: %s\n",
        b->name->c_str(b->name),b->author->c_str(b->author),b->publisher->c_str(b->publisher),b->time->c_str(b->time),b->status==0?"可借":"已借出");
    printf("请选择:\n");
}

void borrow_book(void *arg){
    struct{
        student s;
        book b;
    } *args=arg;

    student s=args->s;
    book b=args->b;
    if(b->status==0&&s->borrowedCount<MAX_AVAILABLE){
        vector borrow_records=load_borrow_records(borrowDb,s->id);
        string record=new_string();
        record->append_n(record,(const char *)&b->id,sizeof(size_t));
        size_t timestamp=(size_t)time(NULL);
        record->append_n(record,(const char *)&timestamp,sizeof(size_t));
        borrow_records->push_back(borrow_records,record);
        save_borrow_records(borrowDb,s->id,borrow_records);
        borrow_records->free(borrow_records);

        s->borrowedCount++;
        b->status=1;
        studentDb->change(studentDb,s->id,s);
        bookDb->change(bookDb,b->id,b);
        printf("借书成功\n");
        getch();
    }
    else{
        printf("无法借书，已达到最大借阅数量或图书已借出。\n");
        getch();
    }
}

void borrow_book_config(void *arg){
    struct{
        student s;
        book b;
    } *args=arg;

    student s=args->s;
    book b=args->b;

    if(s&&b){
        const wchar_t *choices[]={
            L"1. 确认借书",
            L"2. 返回"
        };
        int n_choices=sizeof(choices)/sizeof(choices[0]);
        void (*funcs[])(void *)={
            display_book_details,
            borrow_book,
            student_postInfo
        };
        struct{
            student s;
            book b;
        } args={ s,b };
        void *args_ptr[]={ &args,&args,s };
        menu(n_choices,choices,funcs,args_ptr);
    }
    else{
        printf("学生或书籍不存在\n");
    }
    getchar();
}

void borrow_book_menu(void *arg){
    student s=(student)arg;
    printf("借书功能\n");

    // 刷新图书数据库内容并展示界面
    void (*funcs[])(void *)={
        student_preInfo,
        borrow_book_config,
        student_postInfo
    };
    bool show=false;
    void *args[]={ s,NULL,s,&show };
    page(bookDb,DEFAULT_PAGE_SIZE,funcs,args);
}