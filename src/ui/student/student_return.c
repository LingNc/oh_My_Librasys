#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "ui/components/page.h"
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "function.h"
#include "ui/student/student_return.h"
#include "ui/student/student_borrow.h"

#define DEFAULT_PAGE_SIZE 10

extern dataBase studentDb,borrowDb,bookDb;

void return_book(void *arg){
    struct {
        manager from;
        struct{
            student s;
            book b;
        }*info;
    } *args = arg;
    manager from = args->from;
    student s = args->info->s;
    book b = args->info->b;

    if (s && b) {
        vector borrow_records = load_borrow_records(borrowDb, s->id);
        for (size_t i = 0; i < borrow_records->size(borrow_records); ++i) {
            string record = (string)borrow_records->at(borrow_records, i);
            size_t borrowed_book_id;
            memcpy(&borrowed_book_id, record->c_str(record), sizeof(size_t));
            if(borrowed_book_id==b->id){
                if(!from&&b->borrowData+MAX_BORROW_TIME<(size_t)time(NULL)){
                    size_t overTime=(time(NULL)-b->borrowData)/86400;
                    printf("已经逾期%zu天，无法归还，请联系管理员\n",overTime);
                    size_t price=overTime/10;
                    printf("并缴纳管理费用：%zu元\n",price);
                    printf("按任意键继续\n");
                    getch();
                    return;
                }
                borrow_records->remove(borrow_records,i);
                save_borrow_records(borrowDb, s->id, borrow_records);
                borrow_records->free(borrow_records);

                s->borrowedCount--;
                b->status=0;
                b->borrowData=0;
                studentDb->change(studentDb,s->id,s);
                bookDb->change(bookDb, b->id, b);
                printf("还书成功\n");
                getch();
                return;
            }
        }
        printf("未找到借阅记录\n");
    } else {
        printf("该条目不存在，或者书籍或学生不存在\n");
    }
    getch();
}

void return_book_config(void *arg) {
    struct {
        student s;
        book b;
    } *args = arg;

    student s = args->s;
    book b = args->b;

    if (s && b) {
        const wchar_t *choices[] = {
            L"1. 确认还书",
            L"2. 返回"
        };
        int n_choices = sizeof(choices) / sizeof(choices[0]);
        void (*funcs[])(void *) = {
            display_book_details,
            return_book,
            student_postInfo
        };
        struct {
            manager from;
            struct{
                student s;
                book b;
            }*info;
        }*return_args=malloc(sizeof(*return_args));
        return_args->from=NULL;
        return_args->info=malloc(sizeof(*return_args->info));
        return_args->info->s=s;
        return_args->info->b=b;

        void *args_ptr[]={ args,return_args,s };
        menu(n_choices, choices, funcs, args_ptr);
    } else {
        printf("学生或书籍不存在\n");
    }
    getchar();
}

void return_book_menu(void *arg) {
    student s = (student)arg;
    printf("还书功能\n");

    // 刷新图书数据库内容并展示界面
    void (*funcs[])(void *) = {
        student_preInfo,
        return_book_config,
        student_postInfo
    };
    bool show=true;
    void *args[]={ s,NULL,s,
        &show, // 是否显示已借书籍
        borrowDb, // 数据库
        s // 当前学生
    };
    page(bookDb,DEFAULT_PAGE_SIZE,funcs,args);
}