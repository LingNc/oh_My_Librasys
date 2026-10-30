/*
 * 借还业务回归测试（不依赖 ncurses）
 *
 * 覆盖：借书、重复借阅、还书、重复还书、逾期判定、
 *       学生自助拒绝逾期、管理员代办、借阅上限。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "models/Book.h"
#include "models/Manager.h"
#include "models/Student.h"
#include "service/borrow.h"

dataBase bookDb, studentDb, managerDb, passwordDb, borrowDb;
database_index btos, stoid;

static int failures = 0;

static void expect(const char *what, int condition) {
    printf("%s: %s\n", condition ? "PASS" : "FAIL", what);
    if (!condition) ++failures;
}

static student make_student(size_t student_id, const char *name) {
    student item = new_student();
    load_student(item, 0, student_id, name, "1班", "计算机学院", 0, "", "");
    studentDb->add(studentDb, item);
    size_t id = item->id;
    free_student(item);
    add_index(stoid, student_id, id);
    save_index(stoid);
    studentDb->save(studentDb);
    return studentDb->find_key(studentDb, id);
}

static book make_book(const char *isbn, const char *name) {
    book item = new_book();
    load_book(item, 0, isbn, name, "作者", "出版社", "2024", 0);
    bookDb->add(bookDb, item);
    size_t id = item->id;
    free_book(item);
    bookDb->save(bookDb);
    return bookDb->find_key(bookDb, id);
}

static void reset_database(void) {
    system("rm -rf /tmp/oh_my_librasys_borrow_test && "
           "mkdir -p /tmp/oh_my_librasys_borrow_test");

    bookDb = database("/tmp/oh_my_librasys_borrow_test/book", Book);
    studentDb = database("/tmp/oh_my_librasys_borrow_test/student", Student);
    managerDb = database("/tmp/oh_my_librasys_borrow_test/manager", Manager);
    passwordDb = database("/tmp/oh_my_librasys_borrow_test/password", String);
    borrowDb = database("/tmp/oh_my_librasys_borrow_test/borrow", String);
    stoid = new_index("/tmp/oh_my_librasys_borrow_test/stoid");
    btos = new_index("/tmp/oh_my_librasys_borrow_test/btos");
}

int main(void) {
    reset_database();

    student user = make_student(20240001, "张三");
    book b1 = make_book("ISBN-1", "书一");
    book b2 = make_book("ISBN-2", "书二");

    borrow_result r = borrow_student_book(user, b1);
    expect("首次借书成功", r.status == BORROW_OK);
    expect("借后图书状态为已借出", b1->status == 1);
    expect("借后学生借阅数为 1", user->borrowedCount == 1);

    vector borrowed = borrow_student_books(user);
    expect("借阅列表长度 1", borrowed->size(borrowed) == 1);
    delete_vector(borrowed);

    r = borrow_student_book(user, b1);
    expect("重复借同一本被拒绝", r.status == BORROW_ALREADY_BORROWED);
    expect("重复借后借阅数不变", user->borrowedCount == 1);

    r = borrow_student_book(user, b2);
    expect("借第二本成功", r.status == BORROW_OK);
    expect("借阅数为 2", user->borrowedCount == 2);

    r = return_student_book(user, b1, false);
    expect("还书成功", r.status == BORROW_OK);
    expect("还后图书可借", b1->status == 0);
    expect("还后借阅时间清零", b1->borrowData == 0);
    expect("还后借阅数为 1", user->borrowedCount == 1);

    r = return_student_book(user, b1, false);
    expect("重复还书报找不到记录", r.status == BORROW_RECORD_NOT_FOUND);

    b2->borrowData = (size_t)(time(NULL) - 40 * 86400);
    bookDb->change(bookDb, b2->id, b2);

    size_t days = 0;
    expect("识别为逾期", borrow_overdue_info(b2, &days, NULL));
    expect("逾期天数约 10 天", days >= 9 && days <= 11);

    r = return_student_book(user, b2, false);
    expect("学生自助还逾期书被拒绝", r.status == BORROW_OVERDUE);
    expect("拒绝后图书仍为已借出", b2->status == 1);

    r = return_student_book(user, b2, true);
    expect("管理员代办可还逾期书", r.status == BORROW_OK);
    expect("代办后图书可借", b2->status == 0);

    book made[10];
    for (int i = 0; i < 10; ++i) {
        char isbn[32], name[32];
        snprintf(isbn, sizeof(isbn), "BULK-%d", i);
        snprintf(name, sizeof(name), "批量书%d", i);
        made[i] = make_book(isbn, name);
        borrow_student_book(user, made[i]);
    }
    expect("已借满 10 本", user->borrowedCount == 10);

    book extra = make_book("EXTRA-1", "超额书");
    r = borrow_student_book(user, extra);
    expect("超过上限被拒绝", r.status == BORROW_MAX_REACHED);

    printf("\n%s (%d failures)\n", failures ? "FAILED" : "ALL PASSED", failures);
    return failures ? 1 : 0;
}
