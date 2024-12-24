#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "ui/components/menu.h"
#include "ui/components/func.h"
#include "ui/components/page.h"
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Manager.h"
#include "function.h"
#include "ui/admin/admin_menu.h"
#include "ui/student/student_return.h"



extern dataBase bookDb, studentDb, borrowDb;
extern database_index btos;

void admin_book_preInfo(void *arg){
    admin_preInfo(arg);
    printf("查看书籍列表功能  图书总数:%zu\n",bookDb->size(bookDb));
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
        b->ISBN->assign_cstr(b->ISBN,ISBN);
        b->name->assign_cstr(b->name,name);
        b->author->assign_cstr(b->author,author);
        b->publisher->assign_cstr(b->publisher,publisher);
        b->time->assign_cstr(b->time,time);
        bookDb->change(bookDb,b->id,b);
        printf("修改图书成功\n");
    } else {
        printf("图书不存在\n");
    }
    getch();
}

void delete_book(void *arg){
    book b=(book)arg;
    if(b){
        if(b->status==1){
            printf("书籍已被借出，是否自动归还?(y/n)\n");
            char a[MAX_INPUT];
            if(!getaline(a,"qyn")){
                if(a[0]=='y'){
                    size_t student_id=find_index(btos,b->id);
                    student s=studentDb->find_key(studentDb,student_id);
                    if(s){
                        printf("强制还书成功\n");
                        vector borrow_records=load_borrow_records(borrowDb,s->id);
                        for(size_t i=0; i<borrow_records->size(borrow_records); ++i){
                            string record=(string)borrow_records->at(borrow_records,i);
                            size_t borrowed_book_id;
                            memcpy(&borrowed_book_id,record->c_str(record),sizeof(size_t));
                            if(borrowed_book_id==b->id){
                                borrow_records->remove(borrow_records,i);
                                save_borrow_records(borrowDb,s->id,borrow_records);
                                borrow_records->free(borrow_records);
                                s->borrowedCount--;
                                studentDb->change(studentDb,s->id,s);
                                break;
                            }
                        }
                    }
                }
                bookDb->rm(bookDb,b->id);
                bookDb->save(bookDb);
                printf("删除图书成功\n");
            }
            else{
                printf("图书不存在\n");
            }
            getch();
        }
    }
}

void view_borrow_info(void *arg){
    struct{
        manager m;
        book b;
    }*args=arg;
    book b=args->b;
    manager m=args->m;
    admin_preInfo(m);

    if(b->status==1){
        size_t student_id = find_index(btos, b->id);
        student s = studentDb->find_key(studentDb, student_id);
        if (s) {
            printf("书籍已被借出\n");
            printf("借阅学生ID: %zu\n", s->id);
            printf("姓名: %s\n", s->name->c_str(s->name));
            printf("班级: %s\n", s->class->c_str(s->class));
            printf("学院: %s\n", s->department->c_str(s->department));
        } else {
            printf("未找到借阅学生信息\n");
        }
    } else {
        printf("当前图书书籍未被借出\n");
    }
}
void admin_book_borrow_menu(void *arg){
    struct{
        manager m;
        book b;
    }*in_args=arg;
    struct{
        student s;
        book b;
    }out_arg;
    out_arg.b=in_args->b;
    size_t student_id=find_index(btos,in_args->b->id);
    out_arg.s=studentDb->find_key(studentDb,student_id);
    const wchar_t *choices[]={
        L"1. 强制还书",
        L"2. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *)={
        view_borrow_info,
        return_book,
        NULL
    };
    void *args_ptr[] = { arg, &out_arg, NULL };
    menu(n_choices, choices, funcs, args_ptr);
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
        L"3. 借阅管理",
        L"4. 返回"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void *) = {
        admin_preInfo,
        edit_book,
        delete_book,
        admin_book_borrow_menu,
        NULL
    };
    void *args_ptr[] = { m, b, b, args, NULL };
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
