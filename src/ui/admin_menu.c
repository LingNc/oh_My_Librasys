#include <stdio.h>
#include <wchar.h>
#include "ui/menu.h"
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Student.h"
#include "function.h"

extern dataBase bookDb, studentDb;

void import_books(){
    clear_screen();
    printf("按q键退出\n");
    printf("批量导入书籍功能\n");
    char file_path[100];
    printf("请输入文件路径: ");
    scanf("%s",file_path);
    if(strcmp(file_path,"q")==0)
        return;

    load_books_from_file(file_path,bookDb);
    printf("批量导入书籍成功\n");
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_book(void **arg) {
    clear_screen();
    printf("增加书功能\n");
    printf("1. 手动输入\n");
    printf("2. 批量导入\n");
    int choice;
    printf("按q键退出\n");
    printf("请选择操作: ");
    scanf("%d", &choice);
    if (choice == 113)
        return;
    putchar('\n');
    if (choice == 1) {
        while (1) {
            size_t id;
            char ISBN[20], name[50], author[50], publisher[50], time[20];
            int status;

            printf("请输入书籍ID: ");
            scanf("%zu", &id);
            printf("请输入ISBN: ");
            scanf("%s", ISBN);
            printf("请输入书名: ");
            scanf("%s", name);
            printf("请输入作者: ");
            scanf("%s", author);
            printf("请输入出版社: ");
            scanf("%s", publisher);
            printf("请输入出版时间: ");
            scanf("%s", time);
            printf("请输入状态(0: 可借, 1: 已借出): ");
            scanf("%d", &status);

            book b = new_book();
            load_book(b, id, ISBN, name, author, publisher, time, status);
            bookDb->add(bookDb, b);
            bookDb->save(bookDb);
            printf("增加书成功\n");
            printf("是否继续添加? (y/n)\n");
            // 清空输入缓冲区
            getchar();
            char a;
            scanf("%s", &a);
            if (a == 'n')
                break;
            clear_screen();
        }

    } else if (choice == 2) {
        import_books();
    } else {
        printf("无效选择\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void delete_book(void **arg) {
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

void view_book_list(void **arg) {
    clear_screen();
    printf("查看书籍列表功能\n");
    size_t page, page_size;
    printf("请输入页码: ");
    scanf("%zu", &page);
    printf("请输入每页显示的书籍数量: ");
    scanf("%zu", &page_size);

    size_t start = (page - 1) * page_size;
    size_t end = start + page_size;
    size_t book_count = bookDb->size(bookDb);

    if (start >= book_count) {
        printf("无效页码\n");
        getchar(); getchar(); // 等待用户按键
        clear_screen();
        return;
    }

    vector books = bookDb->get(bookDb, start, page_size);
    printf("%-10s %-20s %-12s %-12s %-12s %-10s\n", "书籍ID", "ISBN", "书名", "作者", "出版社", "状态");
    for (size_t i = 0; i < books->size(books); i++) {
        book b = books->at(books, i);
        if (b) {
            printf("%-10zu %-20s %-12.12s %-12.12s %-12.12s %-10s\n",
                   b->id, b->ISBN->c_str(b->ISBN), b->name->c_str(b->name), b->author->c_str(b->author),
                   b->publisher->c_str(b->publisher), b->status == 0 ? "可借" : "已借出");
        }
    }
    books->free(books);
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void add_student(void **arg) {
    clear_screen();
    printf("增加学生功能\n");
    while (1) {
        size_t id;
        char name[50], class[50], department[50];
        int borrowedCount;
        char borrowedDate[20], returnDate[20];

        printf("请输入学生ID: ");
        scanf("%zu", &id);
        printf("请输入姓名: ");
        scanf("%s", name);
        printf("请输入班级: ");
        scanf("%s", class);
        printf("请输入学院: ");
        scanf("%s", department);
        printf("请输入借阅数量: ");
        scanf("%d", &borrowedCount);
        printf("请输入借阅日期: ");
        scanf("%s", borrowedDate);
        printf("请输入归还日期: ");
        scanf("%s", returnDate);

        student s = new_student();
        load_student(s, id, name, class, department, borrowedCount, borrowedDate, returnDate);
        studentDb->add(studentDb, s);
        studentDb->save(studentDb);
        printf("增加学生成功\n");
        printf("是否继续添加? (y/n)\n");
        // 清空输入缓冲区
        getchar();
        char a;
        scanf("%s", &a);
        if (a == 'n')
            break;
        clear_screen();
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void delete_student(void **arg) {
    clear_screen();
    printf("删除学生功能\n");
    size_t student_id;
    printf("按q退出\n");
    printf("请输入学生ID: ");
    scanf("%zu", &student_id);

    student s = studentDb->find_key(studentDb, student_id);
    if (s) {
        studentDb->rm(studentDb, student_id);
        studentDb->save(studentDb);
        printf("删除学生成功\n");
    } else {
        printf("学生不存在\n");
    }
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void view_student_list(void **arg) {
    clear_screen();
    printf("查看学生列表功能\n");
    size_t page, page_size;
    printf("请输入页码: ");
    scanf("%zu", &page);
    printf("请输入每页显示的学生数量: ");
    scanf("%zu", &page_size);

    size_t start = (page - 1) * page_size;
    size_t end = start + page_size;
    size_t student_count = studentDb->size(studentDb);

    if (start >= student_count) {
        printf("无效页码\n");
        getchar(); getchar(); // 等待用户按键
        clear_screen();
        return;
    }

    vector students = studentDb->get(studentDb, start, page_size);
    printf("%-10s %-20s %-12s %-12s %-10s\n", "学生ID", "姓名", "班级", "学院", "借阅数量");
    for (size_t i = 0; i < students->size(students); i++) {
        student s = students->at(students, i);
        if (s) {
            printf("%-10zu %-20s %-12.12s %-12.12s %-10d\n",
                   s->id, s->name->c_str(s->name), s->class->c_str(s->class), s->department->c_str(s->department), s->borrowedCount);
        }
    }
    students->free(students);
    getchar(); getchar(); // 等待用户按键
    clear_screen();
}

void admin_menu(void **arg) {
    const wchar_t *choices[] = {
        L"1. 增加书",
        L"2. 删除书",
        L"3. 查看书籍列表",
        L"4. 添加学生",
        L"5. 删除学生",
        L"6. 查看学生列表",
        L"7. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void (*funcs[])(void **) = {
        add_book,
        delete_book,
        view_book_list,
        add_student,
        delete_student,
        view_student_list
    };
    menu(choices, funcs, n_choices, NULL);
}