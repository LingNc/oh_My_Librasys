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
#include "ui/student_return.h"
#include "ui/student_borrow.h"

#define DEFAULT_PAGE_SIZE 10

extern dataBase studentDb,borrowDb,bookDb;

void return_book(void *arg) {
    struct {
        student s;
        book b;
    } *args = arg;

    student s = args->s;
    book b = args->b;

    if (s && b) {
        vector borrow_records = load_borrow_records(borrowDb, s->id);
        for (size_t i = 0; i < borrow_records->size(borrow_records); ++i) {
            string record = (string)borrow_records->at(borrow_records, i);
            size_t borrowed_book_id;
            memcpy(&borrowed_book_id, record->c_str(record), sizeof(size_t));
            if (borrowed_book_id == b->id) {
                borrow_records->remove(borrow_records, i);
                save_borrow_records(borrowDb, s->id, borrow_records);
                borrow_records->free(borrow_records);

                s->borrowedCount--;
                b->status = 0;
                studentDb->save(studentDb);
                bookDb->save(bookDb);
                printf("还书成功\n");
                return;
            }
        }
        printf("未找到借阅记录\n");
    } else {
        printf("学生或书籍不存在\n");
    }
    getchar();
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
            student s;
            book b;
        } args = { s, b };
        void *args_ptr[] = { &args, &args, s };
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
    void *args[] = { s, NULL, s };
    page(bookDb, DEFAULT_PAGE_SIZE, funcs, args);
}