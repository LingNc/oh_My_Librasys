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
#include "models/Manager.h"

extern dataBase bookDb;

void admin_book_preInfo(void *arg) {
    admin_preInfo(arg);
    printf("查看书籍列表功能\n");
}

void admin_book_postInfo(void *arg) {
    printf("\n输入/help查看帮助\n");
}

void import_books(void *arg) {
    clear_screen();
    printf("按q键退出\n");
    printf("批量导入书籍功能\n");
    char file_path[100];
    printf("请输入文件路径: ");
    if (!getaline(file_path, "q")) {
        return;
    }
    bool res = load_books_from_file(file_path, bookDb);
    if (res) printf("批量导入书籍成功\n");
    else printf("批量导入书籍失败\n");
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_book_manual(void *arg) {
    while (1) {
        clear_screen();
        printf("增加图书功能\n");
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
        if (a[0] != 'y') break;
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
    book b = (book)arg;

    if (b) {
        clear_screen();
        printf("修改图书功能\n");
        char ISBN[20], name[50], author[50], publisher[50], time[20];

        printf("当前ISBN: %s\n", b->ISBN->c_str(b->ISBN));
        printf("请输入新的ISBN: ");
        if (!getaline(ISBN, "q")) {
            return;
        }
        printf("\n当前书名: %s\n", b->name->c_str(b->name));
        printf("请输入新的书名: ");
        if (!getaline(name, "q")) {
            return;
        }
        printf("\n当前作者: %s\n", b->author->c_str(b->author));
        printf("请输入新的作者: ");
        if (!getaline(author, "q")) {
            return;
        }
        printf("\n当前出版社: %s\n", b->publisher->c_str(b->publisher));
        printf("请输入新的出版社: ");
        if (!getaline(publisher, "q")) {
            return;
        }
        printf("\n当前出版时间: %s\n", b->time->c_str(b->time));
        printf("请输入新的出版时间: ");
        if (!getaline(time, "q")) {
            return;
        }
        b->ISBN->assign_cstr(b->ISBN,ISBN);
        b->name->assign_cstr(b->name,name);
        b->author->assign_cstr(b->author,author);
        b->publisher->assign_cstr(b->publisher,publisher);
        b->time->assign_cstr(b->time,time);
        bookDb->change(bookDb,b->id,b);
        printf("\n修改图书成功\n");
    } else {
        printf("\n图书不存在\n");
    }
    getch();
}

void delete_book(void *arg) {
    book b = (book)arg;

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
    struct {
        manager m;
        book b;
    } *args = arg;
    book b = args->b;
    manager m = args->m;
    const wchar_t *choices[] = {
        L"1. 修改图书",
        L"2. 删除图书",
        L"3. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_preInfo,
        edit_book,
        delete_book,
        NULL
    };
    void *args_ptr[] = { m, b, b, NULL };
    menu(n_choices, choices, funcs, args_ptr);
}

void view_book_list(void *arg) {
    void (*funcs[])(void *) = {
        admin_book_preInfo,
        book_menu,
        admin_book_postInfo
    };
    bool show = false;
    void *args[] = { arg, NULL, arg, &show };
    page(bookDb, 10, funcs, args);
}
