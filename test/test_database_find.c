#include <stdio.h>
#include <stdlib.h>

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "models/Book.h"
#include "models/Manager.h"
#include "models/Student.h"

dataBase bookDb, studentDb, managerDb, passwordDb, borrowDb;
database_index btos, stoid;

static int failures = 0;

static void expect(const char *what, int condition) {
    printf("%s: %s\n", condition ? "PASS" : "FAIL", what);
    if (!condition) ++failures;
}

static void make_book(const char *isbn, const char *name, const char *publisher) {
    book item = new_book();
    load_book(item, 0, isbn, name, "作者", publisher, "2024", 0);
    bookDb->add(bookDb, item);
    free_book(item);
    bookDb->save(bookDb);
}

static size_t count_matches(void) {
    return bookDb->_find_buffer->size(bookDb->_find_buffer);
}

int main(void) {
    system("rm -rf /tmp/vfind2 && mkdir -p /tmp/vfind2");

    bookDb = database("/tmp/vfind2/book", Book);
    studentDb = database("/tmp/vfind2/student", Student);
    managerDb = database("/tmp/vfind2/manager", Manager);
    passwordDb = database("/tmp/vfind2/password", String);
    borrowDb = database("/tmp/vfind2/borrow", String);
    stoid = new_index("/tmp/vfind2/stoid");
    btos = new_index("/tmp/vfind2/btos");

    make_book("ISBN-A", "书一", "清华出版社");
    make_book("ISBN-B", "书二", "清华出版社");
    make_book("ISBN-C", "书三", "机械出版社");

    /* 单条精确匹配 */
    book c1 = new_book();
    load_book(c1, 0, "ISBN-B", "", "", "", "", 0);
    size_t next = bookDb->find(bookDb, 100, c1);
    expect("精确匹配只返回 1 条", count_matches() == 1);
    expect("命中时游标非 0", next != 0);
    free_book(c1);

    /* 多条匹配（只限定出版社） */
    book c2 = new_book();
    load_book(c2, 0, "", "", "", "清华出版社", "", 0);
    bookDb->find(bookDb, 100, c2);
    expect("按出版社匹配返回 2 条", count_matches() == 2);
    free_book(c2);

    /* nums 限制 */
    book c3 = new_book();
    load_book(c3, 0, "", "", "", "清华出版社", "", 0);
    bookDb->find(bookDb, 1, c3);
    expect("nums=1 时只取 1 条", count_matches() == 1);
    free_book(c3);

    /* 无匹配：必须返回 0，否则 get_find 会死循环 */
    book c4 = new_book();
    load_book(c4, 0, "NOT-EXIST", "", "", "", "", 0);
    next = bookDb->find(bookDb, 100, c4);
    expect("无匹配返回 0", next == 0);
    expect("无匹配时缓冲区为空", count_matches() == 0);
    free_book(c4);

    /* 多次调用不应累积（buffer 每次清空） */
    book c5 = new_book();
    load_book(c5, 0, "", "", "", "清华出版社", "", 0);
    bookDb->find(bookDb, 100, c5);
    bookDb->find(bookDb, 100, c5);
    expect("重复调用不累积结果", count_matches() == 2);
    free_book(c5);

    /* 空条件（所有字段为空）应命中全部 */
    book c6 = new_book();
    bookDb->find(bookDb, 100, c6);
    expect("空条件命中全部 3 条", count_matches() == 3);
    free_book(c6);

    /*
     * 注意：这里不做 close_database / close_index。
     * 现有释放路径本身会崩溃（见文档已知问题 #4），
     * 进程退出时由系统回收即可。
     */

    printf("\n%s (%d failures)\n", failures ? "FAILED" : "ALL PASSED", failures);
    return failures ? 1 : 0;
}
