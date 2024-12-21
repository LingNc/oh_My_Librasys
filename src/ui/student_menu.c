#include <stdio.h>
#include <wchar.h>
#include <time.h>
#include "ui/menu.h"
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Student.h"
#include "function.h"

extern dataBase studentDb, borrowDb, bookDb;

void borrow_book(void *arg) {
    size_t student_id = *(size_t *)arg;
    clear_screen();
    printf("借书功能\n");
    size_t book_id;
    printf("请输入书籍ID: ");
    scanf("%zu", &book_id);

    student s = studentDb->find_key(studentDb, student_id);
    book b = bookDb->find_key(bookDb, book_id);

    if (s && b) {
        if (b->status == 0 && s->borrowedCount < MAX_AVAILABLE) {
            vector borrow_records = load_borrow_records(borrowDb, s->id);
            string record = new_string();
            record->append_n(record, (const char *)&b->id, sizeof(size_t));
            size_t timestamp = time(NULL);
            record->append_n(record, (const char *)&timestamp, sizeof(size_t));
            borrow_records->push_back(borrow_records, record);
            save_borrow_records(borrowDb, s->id, borrow_records);
            borrow_records->free(borrow_records);

            s->borrowedCount++;
            b->status = 1;
            bookDb->change(bookDb, b->id, b);
            studentDb->change(studentDb, s->id, s);
            printf("借书成功\n");
        } else {
            printf("无法借书，书籍已借出或学生已达到最大借阅数量\n");
        }
    } else {
        printf("学生或书籍不存在\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void return_book(void *arg) {
    clear_screen();
    printf("还书功能\n");
    size_t student_id, book_id;
    printf("请输入学生ID: ");
    scanf("%zu", &student_id);
    printf("请输入书籍ID: ");
    scanf("%zu", &book_id);

    student s = studentDb->find_key(studentDb, student_id);
    book b = bookDb->find_key(bookDb, book_id);

    if (s && b) {
        vector borrow_records = load_borrow_records(borrowDb, s->id);
        for (size_t i = 0; i < borrow_records->size(borrow_records); ++i) {
            string record = (string)borrow_records->at(borrow_records, i);
            size_t book_id;
            memcpy(&book_id, record->c_str(record), sizeof(size_t));
            if (book_id == b->id) {
                borrow_records->remove(borrow_records, i);
                save_borrow_records(borrowDb, s->id, borrow_records);
                borrow_records->free(borrow_records);
                s->borrowedCount--;
                b->status = 0;
                bookDb->save(bookDb);
                studentDb->save(studentDb);
                printf("还书成功\n");
                return;
            }
        }
        printf("未找到借阅的图书。\n");
        borrow_records->free(borrow_records);
    } else {
        printf("学生或书籍不存在\n");
    }
    getchar(); getchar();
    clear_screen();
}

void view_borrow_info(void *arg) {
    size_t student_id = *(size_t *)arg;
    clear_screen();
    printf("查看借阅信息功能\n");

    student s = studentDb->find_key(studentDb, student_id);
    if (s) {
        printf("学生ID: %zu\n", s->id);
        printf("姓名: %s\n", s->name->c_str(s->name));
        printf("班级: %s\n", s->class->c_str(s->class));
        printf("学院: %s\n", s->department->c_str(s->department));
        printf("借阅数量: %d\n", s->borrowedCount);
        vector borrow_records = load_borrow_records(borrowDb, s->id);
        for (size_t i = 0; i < borrow_records->size(borrow_records); ++i) {
            string record = (string)borrow_records->at(borrow_records, i);
            size_t book_id;
            size_t timestamp;
            memcpy(&book_id, record->c_str(record), sizeof(size_t));
            memcpy(&timestamp, record->c_str(record) + sizeof(size_t), sizeof(size_t));
            book b = bookDb->find_key(bookDb, book_id);
            if (b) {
                printf("书籍ID: %zu, 书名: %s, 借出时间: %s", b->id, b->name->c_str(b->name), ctime(&timestamp));
            }
        }
        borrow_records->free(borrow_records);
    } else {
        printf("学生不存在\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void student_menu(void *arg) {
    size_t student_id = *(size_t *)arg;
    const wchar_t *choices[] = {
        L"1. 借书",
        L"2. 还书",
        L"3. 查看借阅信息",
        L"4. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        borrow_book,
        return_book,
        view_borrow_info
    };
    menu(choices, funcs, n_choices, &student_id);
}