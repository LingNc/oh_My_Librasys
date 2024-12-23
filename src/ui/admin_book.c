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
        char ISBN[20], name[50], author[50], publisher[50], time[20];
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
        load_book(b, 0, ISBN, name, author, publisher, time, 0);
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

void edit_book(void *arg) {
    struct {
        book b;
    } *args = arg;

    book b = args->b;

    if (b) {
        clear_screen();
        printf("修改图书功能\n");
        char ISBN[20], name[50], author[50], publisher[50], time[20];

        printf("当前ISBN: %s\n", b->ISBN->c_str(b->ISBN));
        printf("请输入新的ISBN: ");
        if (!getaline(ISBN, "q")) {
            return;
        }

        printf("当前书名: %s\n", b->name->c_str(b->name));
        printf("请输入新的书名: ");
        if (!getaline(name, "q")) {
            return;
        }

        printf("当前作者: %s\n", b->author->c_str(b->author));
        printf("请输入新的作者: ");
        if (!getaline(author, "q")) {
            return;
        }

        printf("当前出版社: %s\n", b->publisher->c_str(b->publisher));
        printf("请输入新的出版社: ");
        if (!getaline(publisher, "q")) {
            return;
        }

        printf("当前出版时间: %s\n", b->time->c_str(b->time));
        printf("请输入新的出版时间: ");
        if (!getaline(time, "q")) {
            return;
        }

        load_book(b, b->id, ISBN, name, author, publisher, time, b->status);
        bookDb->save(bookDb);
        printf("修改图书成功\n");
    } else {
        printf("图书不存在\n");
    }
    getchar();
}

void delete_book(void *arg) {
    struct {
        book b;
    } *args = arg;

    book b = args->b;

    if (b) {
        bookDb->rm(bookDb, b->id);
        bookDb->save(bookDb);
        printf("删除图书成功\n");
    } else {
        printf("图书不存在\n");
    }
    getchar();
}

void book_menu(void *arg) {
    book b = (book)arg;
    const wchar_t *choices[] = {
        L"1. 修改图书",
        L"2. 删除图书",
        L"3. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        edit_book,
        delete_book,
        NULL
    };
    struct {
        book b;
    } args = { b };
    void *args_ptr[] = { &args, &args, NULL };
    menu(n_choices, choices, funcs, args_ptr);
}

void admin_book_preInfo(void *arg) {
    printf("查看书籍列表功能\n");
}

void admin_book_postInfo(void *arg) {
    printf("\n输入/help查看帮助\n");
}

void display_book_list(void *arg) {
    vector books = (vector)arg;
    if (books->size(books) == 0) {
        printf("没有图书信息\n");
        return;
    }
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
    void (*funcs[])(void *) = {
        admin_book_preInfo,
        display_book_list,
        admin_book_postInfo
    };
    bool show = false;
    void *args[] = { arg, NULL, arg, &show };
    page(bookDb, 10, funcs, args);
}
