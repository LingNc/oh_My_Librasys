#include "DataBase/DataBase.h"
#include "uiBook.h"
#include "Book.h"
#include "Student.h"
#include "function.h"
#include "ui/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "time.h"

dataBase bookDb,studentDb,borrowDb;

void clear_screen() {
    printf("\033[H\033[J");
}

void display_student_menu(int highlight) {
    clear_screen();
    const wchar_t *choices[] = {
        L"1. 借书",
        L"2. 还书",
        L"3. 查看借阅信息",
        L"4. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);

    for (int i = 0; i < n_choices; ++i) {
        if (i == highlight) {
            printf("> %ls\n", choices[i]);
        } else {
            printf("  %ls\n", choices[i]);
        }
    }
}

void display_book_management_menu(int highlight) {
    clear_screen();
    const wchar_t *choices[] = {
        L"1. 增加书",
        L"2. 删除书",
        L"3. 查看书籍列表",
        L"4. 添加学生",
        L"5. 删除学生",
        L"6. ��看学生列表",
        L"7. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);

    for (int i = 0; i < n_choices; ++i) {
        if (i == highlight) {
            printf("> %ls\n", choices[i]);
        } else {
            printf("  %ls\n", choices[i]);
        }
    }
}

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

void add_book(void *arg) {
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

void view_book_list(void *arg) {
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

void student_menu(void *arg) {
    size_t student_id = *(size_t *)arg;
    const wchar_t *choices[] = {
        L"1. 借书",
        L"2. 还书",
        L"3. 查看借阅信息",
        L"4. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void *funcs[] = {
        borrow_book,
        return_book,
        view_borrow_info
    };
    menu(choices, funcs, n_choices, &student_id);
}

void add_student(void *arg) {
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

void delete_student(void *arg) {
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

void view_student_list(void *arg) {
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

void book_management_menu(void *arg) {
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
    void *funcs[] = {
        add_book,
        delete_book,
        view_book_list,
        add_student,
        delete_student,
        view_student_list
    };
    menu(choices, funcs, n_choices, NULL);
}

void main_menu() {
    const wchar_t *choices[] = {
        L"1. 管理书",
        L"2. 学生目录",
        L"3. 退出"
    };
    int n_choices = sizeof(choices) / sizeof(choices[0]);
    void *funcs[] = {
        book_management_menu,
        student_menu
    };
    menu(choices, funcs, n_choices, NULL);
}

int main() {
    setlocale(LC_ALL, "");

    bookDb = database("db/book", Book);
    studentDb = database("db/student", Student);
    borrowDb = database("db/borrow_records", String);

    main_menu();

    return 0;
}
