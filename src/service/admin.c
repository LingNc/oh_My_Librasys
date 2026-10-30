#include "service/admin.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "Tools/Hash.h"
#include "Tools/String.h"
#include "function.h"

extern dataBase bookDb, studentDb, managerDb, passwordDb, borrowDb;
extern database_index btos, stoid;

static admin_op_result result(admin_op_status status, const char *message) {
    admin_op_result value = { status, message };
    return value;
}

static int empty(const char *value) {
    return !value || !value[0];
}

static vector list_database(dataBase db) {
    vector items = new_vector(db->_type->c_str(db->_type));
    for (size_t key = 1; key <= db->_index->_last_key; ++key) {
        void *item = db->find_key(db, key);
        if (!item) continue;
        items->push_back(items, item);
        if (strcmp(db->_type->c_str(db->_type), "Book") == 0) free_book(item);
        else if (strcmp(db->_type->c_str(db->_type), "Student") == 0) free_student(item);
        else if (strcmp(db->_type->c_str(db->_type), "Manager") == 0) free_manager(item);
    }
    return items;
}

vector admin_list_books(void) {
    return list_database(bookDb);
}

vector admin_list_students(void) {
    return list_database(studentDb);
}

vector admin_list_managers(void) {
    return list_database(managerDb);
}

admin_op_result admin_add_book(const char *isbn, const char *name,
                               const char *author, const char *publisher,
                               const char *published_at) {
    if (empty(isbn) || empty(name) || empty(author) || empty(publisher) || empty(published_at)) {
        return result(ADMIN_OP_INVALID, "图书字段不能为空");
    }

    book item = new_book();
    load_book(item, 0, isbn, name, author, publisher, published_at, 0);
    bookDb->add(bookDb, item);
    bookDb->save(bookDb);
    free_book(item);
    return result(ADMIN_OP_OK, "增加图书成功");
}

admin_op_result admin_update_book(book item, const char *isbn, const char *name,
                                  const char *author, const char *publisher,
                                  const char *published_at) {
    if (!item) return result(ADMIN_OP_NOT_FOUND, "图书不存在");
    if (empty(isbn) || empty(name) || empty(author) || empty(publisher) || empty(published_at)) {
        return result(ADMIN_OP_INVALID, "图书字段不能为空");
    }

    item->ISBN->assign_cstr(item->ISBN, isbn);
    item->name->assign_cstr(item->name, name);
    item->author->assign_cstr(item->author, author);
    item->publisher->assign_cstr(item->publisher, publisher);
    item->time->assign_cstr(item->time, published_at);
    bookDb->change(bookDb, item->id, item);
    return result(ADMIN_OP_OK, "修改图书成功");
}

admin_op_result admin_delete_book(book item) {
    if (!item) return result(ADMIN_OP_NOT_FOUND, "图书不存在");
    if (item->status == 1) return result(ADMIN_OP_BUSY, "图书仍在借阅中，不能删除");
    bookDb->rm(bookDb, item->id);
    bookDb->save(bookDb);
    return result(ADMIN_OP_OK, "删除图书成功");
}

student admin_book_borrower(book item) {
    if (!item || item->status != 1) return NULL;
    size_t student_id = find_index(btos, item->id);
    if (student_id == 0) return NULL;
    return studentDb->find_key(studentDb, student_id);
}

admin_op_result admin_force_return(book item) {
    student borrower = admin_book_borrower(item);
    if (!item || !borrower) return result(ADMIN_OP_NOT_FOUND, "未找到借阅学生或借阅记录");

    vector records = load_borrow_records(borrowDb, borrower->id);
    for (size_t i = 0; i < records->size(records); ++i) {
        string record = records->at(records, i);
        size_t book_id = 0;
        memcpy(&book_id, record->c_str(record), sizeof(book_id));
        if (book_id != item->id) continue;

        records->remove(records, i);
        save_borrow_records(borrowDb, borrower->id, records);
        records->free(records);
        borrower->borrowedCount--;
        item->status = 0;
        item->borrowData = 0;
        studentDb->change(studentDb, borrower->id, borrower);
        bookDb->change(bookDb, item->id, item);
        return result(ADMIN_OP_OK, "强制还书成功");
    }

    records->free(records);
    return result(ADMIN_OP_NOT_FOUND, "未找到借阅记录");
}

admin_op_result admin_add_student(size_t student_id, const char *name,
                                  const char *class_name, const char *department) {
    if (student_id == 0 || empty(name) || empty(class_name) || empty(department)) {
        return result(ADMIN_OP_INVALID, "学生字段不能为空");
    }
    if (find_index(stoid, student_id) != 0) {
        return result(ADMIN_OP_DUPLICATE, "学生学号已存在");
    }

    student item = new_student();
    load_student(item, 0, student_id, name, class_name, department, 0, "", "");
    studentDb->add(studentDb, item);
    studentDb->save(studentDb);
    add_index(stoid, item->studentID, item->id);
    save_index(stoid);
    free_student(item);
    return result(ADMIN_OP_OK, "增加学生成功");
}

admin_op_result admin_update_student(student item, const char *name,
                                     const char *class_name, const char *department) {
    if (!item) return result(ADMIN_OP_NOT_FOUND, "学生不存在");
    if (empty(name) || empty(class_name) || empty(department)) {
        return result(ADMIN_OP_INVALID, "学生字段不能为空");
    }
    item->name->assign_cstr(item->name, name);
    item->class->assign_cstr(item->class, class_name);
    item->department->assign_cstr(item->department, department);
    studentDb->change(studentDb, item->id, item);
    return result(ADMIN_OP_OK, "修改学生成功");
}

admin_op_result admin_delete_student(student item) {
    if (!item) return result(ADMIN_OP_NOT_FOUND, "学生不存在");
    if (item->borrowedCount > 0) return result(ADMIN_OP_BUSY, "学生仍有未归还图书，不能删除");
    studentDb->rm(studentDb, item->id);
    studentDb->save(studentDb);
    remove_index(stoid, item->studentID);
    save_index(stoid);
    return result(ADMIN_OP_OK, "删除学生成功");
}

admin_op_result admin_add_manager(manager current, size_t id, const char *name,
                                  const char *password) {
    if (!current || id == 0 || empty(name) || empty(password)) {
        return result(ADMIN_OP_INVALID, "管理员字段不能为空");
    }
    manager existing = managerDb->find_key(managerDb, id);
    if (existing) {
        free_manager(existing);
        return result(ADMIN_OP_DUPLICATE, "管理员 ID 已存在");
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char date[32] = "";
    if (local) strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", local);

    manager item = new_manager();
    load_manager(item, id, name, date, current->name->c_str(current->name));
    managerDb->add_key(managerDb, item, id);
    managerDb->save(managerDb);

    string password_string = new_string();
    password_string->assign_cstr(password_string, password);
    string hash = sha256(password_string);
    passwordDb->add_key(passwordDb, hash, id);
    passwordDb->save(passwordDb);
    hash->free(hash);
    password_string->free(password_string);
    free_manager(item);
    return result(ADMIN_OP_OK, "增加管理员成功");
}

static int can_manage(manager current, manager item) {
    return current && item && (current->id == 1 ||
        item->registered_by->cmp(item->registered_by, current->name) == 0);
}

admin_op_result admin_update_manager(manager current, manager item, const char *name) {
    if (!item) return result(ADMIN_OP_NOT_FOUND, "管理员不存在");
    if (!can_manage(current, item)) return result(ADMIN_OP_INVALID, "权限不足");
    if (empty(name)) return result(ADMIN_OP_INVALID, "姓名不能为空");
    item->name->assign_cstr(item->name, name);
    managerDb->change(managerDb, item->id, item);
    return result(ADMIN_OP_OK, "修改管理员成功");
}

admin_op_result admin_delete_manager(manager current, manager item) {
    if (!item) return result(ADMIN_OP_NOT_FOUND, "管理员不存在");
    if (item->id == 1) return result(ADMIN_OP_INVALID, "不能删除 root 管理员");
    if (!can_manage(current, item)) return result(ADMIN_OP_INVALID, "权限不足");
    managerDb->rm(managerDb, item->id);
    managerDb->save(managerDb);
    passwordDb->rm(passwordDb, item->id);
    passwordDb->save(passwordDb);
    return result(ADMIN_OP_OK, "删除管理员成功");
}

admin_op_result admin_change_password(manager current, manager item,
                                      const char *password, const char *confirmation) {
    if (!item || !current) return result(ADMIN_OP_NOT_FOUND, "管理员不存在");
    if (current->id != item->id) return result(ADMIN_OP_INVALID, "只能修改当前管理员密码");
    if (empty(password) || empty(confirmation)) return result(ADMIN_OP_INVALID, "密码不能为空");
    if (strcmp(password, confirmation) != 0) return result(ADMIN_OP_PASSWORD_MISMATCH, "两次密码不一致");

    string value = new_string();
    value->assign_cstr(value, password);
    string hash = sha256(value);
    passwordDb->change(passwordDb, item->id, hash);
    passwordDb->save(passwordDb);
    hash->free(hash);
    value->free(value);
    return result(ADMIN_OP_OK, "密码修改成功");
}