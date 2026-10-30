#include "service/borrow.h"

#include <string.h>
#include <time.h>

#include "DataBase/DataBase.h"
#include "Tools/String.h"
#include "function.h"

extern dataBase bookDb, studentDb, borrowDb;

/* 与 ui/components/func.h 的 MAX_BORROW_TIME 保持一致（30 天） */
#define BORROW_PERIOD_SECONDS (30 * 86400)

static borrow_result make_result(borrow_status status, const char *message) {
    borrow_result result = { status, message };
    return result;
}

bool borrow_overdue_info(book item, size_t *days, size_t *hours) {
    if (!item || item->status != 1) return false;

    time_t now = time(NULL);
    time_t due = (time_t)item->borrowData + BORROW_PERIOD_SECONDS;
    if (now <= due) return false;

    time_t passed = now - due;
    if (days) *days = (size_t)(passed / 86400);
    if (hours) *hours = (size_t)((passed % 86400) / 3600);
    return true;
}

static size_t find_record(vector records, size_t book_id) {
    for (size_t i = 0; i < records->size(records); ++i) {
        string record = records->at(records, i);
        size_t id = 0;
        memcpy(&id, record->c_str(record), sizeof(id));
        if (id == book_id) return i;
    }
    return (size_t)-1;
}

borrow_result borrow_student_book(student user, book item) {
    if (!user) return make_result(BORROW_STUDENT_NOT_FOUND, "学生不存在");
    if (!item) return make_result(BORROW_BOOK_NOT_FOUND, "图书不存在");
    if (item->status == 1) return make_result(BORROW_ALREADY_BORROWED, "该图书已被借出");
    if (user->borrowedCount >= MAX_AVAILABLE) {
        return make_result(BORROW_MAX_REACHED, "已达到最大借阅数量，请先还书");
    }

    vector records = load_borrow_records(borrowDb, user->id);
    if (find_record(records, item->id) != (size_t)-1) {
        delete_vector(records);
        return make_result(BORROW_ALREADY_BORROWED, "你已借阅该图书");
    }

    string record = new_string();
    size_t timestamp = (size_t)time(NULL);
    record->append_n(record, (const char *)&item->id, sizeof(size_t));
    record->append_n(record, (const char *)&timestamp, sizeof(size_t));
    records->push_back(records, record);
    delete_string(record);
    save_borrow_records(borrowDb, user->id, records);
    delete_vector(records);

    user->borrowedCount++;
    item->status = 1;
    item->borrowData = timestamp;
    studentDb->change(studentDb, user->id, user);
    bookDb->change(bookDb, item->id, item);
    return make_result(BORROW_OK, "借书成功");
}

borrow_result return_student_book(student user, book item, bool allow_overdue) {
    if (!user) return make_result(BORROW_STUDENT_NOT_FOUND, "学生不存在");
    if (!item) return make_result(BORROW_BOOK_NOT_FOUND, "图书不存在");

    vector records = load_borrow_records(borrowDb, user->id);
    size_t position = find_record(records, item->id);
    if (position == (size_t)-1) {
        delete_vector(records);
        return make_result(BORROW_RECORD_NOT_FOUND, "未找到该图书的借阅记录");
    }

    if (!allow_overdue && borrow_overdue_info(item, NULL, NULL)) {
        delete_vector(records);
        return make_result(BORROW_OVERDUE, "图书已逾期，请联系管理员归还");
    }

    records->remove(records, position);
    save_borrow_records(borrowDb, user->id, records);
    delete_vector(records);

    if (user->borrowedCount > 0) user->borrowedCount--;
    item->status = 0;
    item->borrowData = 0;
    studentDb->change(studentDb, user->id, user);
    bookDb->change(bookDb, item->id, item);
    return make_result(BORROW_OK, "还书成功");
}

size_t borrow_student_book_count(student user) {
    if (!user) return 0;
    vector records = load_borrow_records(borrowDb, user->id);
    size_t count = records->size(records);
    delete_vector(records);
    return count;
}

vector borrow_student_books(student user) {
    vector result = new_vector("Book");
    if (!user) return result;

    vector records = load_borrow_records(borrowDb, user->id);
    for (size_t i = 0; i < records->size(records); ++i) {
        string record = records->at(records, i);
        size_t book_id = 0;
        memcpy(&book_id, record->c_str(record), sizeof(book_id));
        book item = bookDb->find_key(bookDb, book_id);
        if (!item) continue;
        result->push_back(result, item);
        free_book(item);
    }
    delete_vector(records);
    return result;
}
