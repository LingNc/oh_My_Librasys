#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "function.h"
#include "ui/admin_menu.h"
#include "ui/components/page.h"

extern dataBase bookDb;

void import_books(){
    clear_screen();
    printf("按q键退出\n");
    printf("批量导入书籍功能\n");
    char file_path[100];
    printf("请输入文件路径: ");
    if(!getaline(file_path,"q"))
        return;

    bool res=load_books_from_file(file_path,bookDb);
    if(res) printf("批量导入书籍成功\n");
    else printf("批量导入书籍失败\n");
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_book_manual(void *arg) {
    while(1){
        clear_screen();
        printf("增加图书功能: \n");
        size_t id;
        char idStr[MAX_INPUT];
        char ISBN[20], name[50], author[50], publisher[50], time[20];

        printf("请输入书籍ID: ");
        if (!getaline(idStr, "q")) {
            return;
        }
        id = (size_t)atoll(idStr);

        printf("请输入ISBN: ");
        if (!getaline(ISBN, "q")) {
            return;
        }

        printf("请输入书名: ");
        if (!getaline(name, "q")) {
            return;
        }

        printf("请输入作者: ");
        if (!getaline(author, "q")) {
            return;
        }

        printf("请输入出版社: ");
        if (!getaline(publisher, "q")) {
            return;
        }

        printf("请输入出版时间: ");
        if (!getaline(time, "q")) {
            return;
        }

        book b = new_book();
        load_book(b, id, ISBN, name, author, publisher, time, 0);
        bookDb->add(bookDb, b);
        bookDb->save(bookDb);
        printf("增加书成功\n");
        printf("是否继续添加? (y/n)\n");
        // 清空输入缓冲区
        char a[MAX_INPUT];
        if (!getaline(a, "qn")) {
            return;
        }
        clear_screen();
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_book(void *arg) {
    const wchar_t *choices[] = {
        L"1. 手动输入",
        L"2. 批量导入",
        L"3. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_preInfo, // preInfo
        add_book_manual,
        import_books,
        NULL  // postInfo
    };
    void *args[] = { arg, NULL, NULL, NULL };
    menu(n_choices, choices, funcs, args);
}

void delete_book(void *arg) {
    clear_screen();
    printf("删除书功能\n");
    size_t book_id;
    printf("按q退出\n");
    printf("请输入书籍ID: ");
    scanf("%zu", &book_id);

    book b = bookDb->find_key(bookDb, book_id);
    if (b) {
        bookDb->rm(bookDb, book_id);
        bookDb->save(bookDb);
        printf("删除书成功\n");
    } else {
        printf("书籍不存在\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void admin_book_preInfo(void *arg) {
    printf("查看书籍列表功能\n");
}

void admin_book_postInfo(void *arg) {
    printf("\n输入/help查看帮助\n");
}

void display_book_list(void *arg) {
    vector books = (vector)arg;
    printf("%-10s %-20s %-12s %-12s %-12s %-10s\n", "书籍ID", "ISBN", "书名", "作者", "出版社", "状态");
    for (size_t i = 0; i < books->size(books); i++) {
        book b = books->at(books, i);
        if (b) {
            printf("%-10zu %-20s %-12.12s %-12.12s %-12.12s %-10s\n",
                   b->id, b->ISBN->c_str(b->ISBN), b->name->c_str(b->name), b->author->c_str(b->author),
                   b->publisher->c_str(b->publisher), b->status == 0 ? "可借" : "已借出");
        }
    }
}

void view_book_list(void *arg) {
    void (*funcs[])(void *) = { admin_book_preInfo, display_book_list, admin_book_postInfo };
    void *args[] = { arg };
    page(bookDb, 10, funcs, args);
}
