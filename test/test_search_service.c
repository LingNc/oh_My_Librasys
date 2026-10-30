/*
 * 搜索业务回归测试（不依赖 ncurses）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "models/Book.h"
#include "models/Manager.h"
#include "models/Student.h"
#include "service/search.h"

dataBase bookDb, studentDb, managerDb, passwordDb, borrowDb;
database_index btos, stoid;

static int failures = 0;

static void expect(const char *what, int condition) {
    printf("%s: %s\n", condition ? "PASS" : "FAIL", what);
    if (!condition) ++failures;
}

static void make_book(const char *isbn, const char *name, const char *author,
                      const char *publisher) {
    book item = new_book();
    load_book(item, 0, isbn, name, author, publisher, "2024", 0);
    bookDb->add(bookDb, item);
    free_book(item);
    bookDb->save(bookDb);
}

static void make_student(size_t student_id, const char *name, const char *class_name,
                         const char *department) {
    student item = new_student();
    load_student(item, 0, student_id, name, class_name, department, 0, "", "");
    studentDb->add(studentDb, item);
    size_t id = item->id;
    free_student(item);
    add_index(stoid, student_id, id);
    save_index(stoid);
    studentDb->save(studentDb);
}

int main(void) {
    system("rm -rf /tmp/oh_my_librasys_search_test && "
           "mkdir -p /tmp/oh_my_librasys_search_test");

    bookDb = database("/tmp/oh_my_librasys_search_test/book", Book);
    studentDb = database("/tmp/oh_my_librasys_search_test/student", Student);
    managerDb = database("/tmp/oh_my_librasys_search_test/manager", Manager);
    passwordDb = database("/tmp/oh_my_librasys_search_test/password", String);
    borrowDb = database("/tmp/oh_my_librasys_search_test/borrow", String);
    stoid = new_index("/tmp/oh_my_librasys_search_test/stoid");
    btos = new_index("/tmp/oh_my_librasys_search_test/btos");

    make_book("ISBN-A", "C程序设计", "谭浩强", "清华出版社");
    make_book("ISBN-B", "数据结构", "严蔚敏", "清华出版社");
    make_book("ISBN-C", "算法导论", "Cormen", "MIT Press");

    make_student(20240001, "张三", "2024级1班", "计算机学院");
    make_student(20240002, "李四", "2024级2班", "数学学院");

    search_keys found = search_books("数据结构", SEARCH_DEFAULT_LIMIT);
    expect("按书名搜索命中 1 条", found.count == 1);
    search_keys_free(&found);

    found = search_books("清华", SEARCH_DEFAULT_LIMIT);
    expect("按出版社搜索命中 2 条", found.count == 2);
    search_keys_free(&found);

    found = search_books("isbn-a", SEARCH_DEFAULT_LIMIT);
    expect("ISBN 搜索大小写不敏感", found.count == 1);
    search_keys_free(&found);

    found = search_books("谭浩强", SEARCH_DEFAULT_LIMIT);
    expect("按作者搜索命中 1 条", found.count == 1);
    search_keys_free(&found);

    found = search_books("不存在的关键字", SEARCH_DEFAULT_LIMIT);
    expect("无匹配返回 0 条", found.count == 0);
    expect("无匹配不算截断", !found.truncated);
    search_keys_free(&found);

    found = search_books("ISBN", 2);
    expect("limit 生效返回 2 条", found.count == 2);
    expect("达到 limit 标记截断", found.truncated);
    search_keys_free(&found);

    found = search_books("", SEARCH_DEFAULT_LIMIT);
    expect("空关键字返回 0 条", found.count == 0);
    search_keys_free(&found);

    found = search_students("20240001", SEARCH_DEFAULT_LIMIT);
    expect("按学号精确搜索命中 1 条", found.count == 1);
    search_keys_free(&found);

    found = search_students("李四", SEARCH_DEFAULT_LIMIT);
    expect("按姓名搜索命中 1 条", found.count == 1);
    search_keys_free(&found);

    found = search_students("计算机学院", SEARCH_DEFAULT_LIMIT);
    expect("按学院搜索命中 1 条", found.count == 1);
    search_keys_free(&found);

    printf("\n%s (%d failures)\n", failures ? "FAILED" : "ALL PASSED", failures);
    return failures ? 1 : 0;
}
