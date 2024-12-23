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

#define DEFAULT_PAGE_SIZE 10

extern dataBase studentDb, borrowDb, bookDb;

void student_preInfo(void *arg) {
    student s = (student)arg;
    printf("学生信息: ID: %zu, 姓名: %s, 班级: %s, 学院: %s, 借阅数量: %d\n",
           s->id, s->name->c_str(s->name), s->class->c_str(s->class), s->department->c_str(s->department), s->borrowedCount);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("当前时间: %02d:%02d\n", t->tm_hour, t->tm_min);
    printf("请选择一个操作:\n");
}

void student_postInfo(void *arg) {
    printf("\n按任意键返回，按'q'退出\n");
}

void display_book_details(book b) {
    clear_screen();
    printf("书籍详情:\n");
    printf("ID: %zu\n", b->id);
    printf("书名: %s\n", b->name->c_str(b->name));
    printf("作者: %s\n", b->author->c_str(b->author));
    printf("出版社: %s\n", b->publisher->c_str(b->publisher));
    printf("出版日期: %s\n", b->time->c_str(b->time));
    printf("状态: %s\n", b->status == 0 ? "可借" : "已借出");
    printf("\n操作:\n");
    printf("1. 借书\n");
    printf("2. 返回\n");
}

void borrow_book(void *arg) {
    student s = (student)arg;
    printf("借书功能\n");

    // 刷新图书数据库内容并展示界面
    void (*funcs[])(void *) = {
        student_preInfo,
        NULL,
        student_postInfo
    };
    int pageNums = 0;
    void *args[] = { arg, &pageNums, arg };
    page(bookDb, DEFAULT_PAGE_SIZE, funcs, args);

    size_t book_id;
    printf("请输入书籍ID: ");
    scanf("%zu", &book_id);

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
            studentDb->save(studentDb);
            bookDb->save(bookDb);
            printf("借书成功\n");
        } else {
            printf("无法借书，已达到最大借阅数量或图书已借出。\n");
        }
    } else {
        printf("学生或书籍不存在\n");
    }
    getchar();
}

void return_book(void *arg) {
    size_t student_id = *(size_t *)arg;
    printf("还书功能\n");

    size_t book_id;
    printf("请输入书籍ID: ");
    scanf("%zu", &book_id);

    student s = studentDb->find_key(studentDb, student_id);
    book b = bookDb->find_key(bookDb, book_id);

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

void view_borrow_info(void **arg) {
    size_t student_id = *(size_t *)arg[0];
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
            time_t timestamp;
            memcpy(&book_id, record->c_str(record), sizeof(size_t));
            memcpy(&timestamp, record->c_str(record) + sizeof(size_t), sizeof(time_t));
            book b = bookDb->find_key(bookDb, book_id);
            if (b) {
                printf("书籍ID: %zu, 书名: %s, 借出时间: %s", b->id, b->name->c_str(b->name), ctime(&timestamp));
            }
        }
        borrow_records->free(borrow_records);
    } else {
        printf("当前学生尚未借书，暂未借阅信息。\n");
    }
    getchar();  // 等待用户按键
    clear_screen();
}
void student_menu(void *arg) {
    student s=(student)arg;
    if (!s) {
        printf("学生不存在\n");
        return;
    }

    const wchar_t *choices[] = {
        L"1. 借书",
        L"2. 还书",
        L"3. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        student_preInfo, // preInfo
        borrow_book,
        return_book,
        student_postInfo  // postInfo
    };
    void *args[] = { s, s, s, NULL };

    menu(n_choices, choices, funcs, args);
}