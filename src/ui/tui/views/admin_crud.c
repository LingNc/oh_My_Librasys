#include "ui/tui/views/admin_crud.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service/admin.h"
#include "service/auth.h"
#include "service/format.h"
#include "ui/tui/components/dialog.h"
#include "ui/tui/components/form.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/components/table.h"

typedef struct {
    vector rows;
    manager current;
} crud_context;

static const char *required(const char *value) {
    return value && value[0] ? NULL : "不能为空";
}

static const char *positive_id(const char *value) {
    if (!value || !value[0]) return "不能为空";
    errno = 0;
    char *end = NULL;
    uintmax_t parsed = strtoumax(value, &end, 10);
    if (errno || !end || *end != '\0' || parsed == 0 || parsed > SIZE_MAX) {
        return "请输入有效的正整数";
    }
    return NULL;
}

static void show_result(admin_op_result result) {
    if (result.status == ADMIN_OP_OK) tui_info("操作成功", "%s", result.message);
    else tui_error("操作失败", "%s", result.message);
}

static bool verify_current_password(manager current) {
    char password[128] = "";
    tui_field field = { "当前密码", password, sizeof(password), TUI_FIELD_MASKED, required };
    if (tui_form("验证管理员身份", &field, 1,
                 "Enter 验证  Esc 取消") != TUI_FORM_OK) {
        return false;
    }
    manager_auth_result auth = auth_login_manager(current->id, password);
    if (auth.status == AUTH_OK) return true;
    tui_error("验证失败", "当前密码不正确");
    return false;
}

static size_t rows_count(void *ctx) {
    return ((crud_context *)ctx)->rows->size(((crud_context *)ctx)->rows);
}

static void book_row(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    book item = ((crud_context *)ctx)->rows->at(((crud_context *)ctx)->rows, row);
    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->id); break;
    case 1: snprintf(out, cap, "%s", item->ISBN->c_str(item->ISBN)); break;
    case 2: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 3: snprintf(out, cap, "%s", item->author->c_str(item->author)); break;
    case 4: snprintf(out, cap, "%s", item->status ? "已借出" : "可借"); break;
    default: out[0] = '\0'; break;
    }
}

static void student_row(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    student item = ((crud_context *)ctx)->rows->at(((crud_context *)ctx)->rows, row);
    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->studentID); break;
    case 1: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 2: snprintf(out, cap, "%s", item->class->c_str(item->class)); break;
    case 3: snprintf(out, cap, "%s", item->department->c_str(item->department)); break;
    case 4: snprintf(out, cap, "%d", item->borrowedCount); break;
    default: out[0] = '\0'; break;
    }
}

static void manager_row(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    manager item = ((crud_context *)ctx)->rows->at(((crud_context *)ctx)->rows, row);
    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->id); break;
    case 1: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 2: snprintf(out, cap, "%s", item->registration_date->c_str(item->registration_date)); break;
    case 3: snprintf(out, cap, "%s", item->registered_by->c_str(item->registered_by)); break;
    default: out[0] = '\0'; break;
    }
}

static bool book_action(void *ctx, size_t row) {
    crud_context *list = ctx;
    book item = list->rows->at(list->rows, row);
    static const char *const options[] = { "1. 修改图书", "2. 删除图书", "3. 返回" };
    tui_menu_spec spec = { .title = "图书操作", .items = options, .count = 3 };
    int choice = tui_menu(&spec);
    if (choice == 0) {
        char isbn[128], name[128], author[128], publisher[128], date[64];
        snprintf(isbn, sizeof(isbn), "%s", item->ISBN->c_str(item->ISBN));
        snprintf(name, sizeof(name), "%s", item->name->c_str(item->name));
        snprintf(author, sizeof(author), "%s", item->author->c_str(item->author));
        snprintf(publisher, sizeof(publisher), "%s", item->publisher->c_str(item->publisher));
        snprintf(date, sizeof(date), "%s", item->time->c_str(item->time));
        tui_field fields[] = {
            { "ISBN", isbn, sizeof(isbn), 0, required },
            { "书名", name, sizeof(name), 0, required },
            { "作者", author, sizeof(author), 0, required },
            { "出版社", publisher, sizeof(publisher), 0, required },
            { "出版时间", date, sizeof(date), 0, required },
        };
        if (tui_form("修改图书", fields, 5, "Tab/↑↓ 切换  Enter 保存  Esc 取消") == TUI_FORM_OK) {
            show_result(admin_update_book(item, isbn, name, author, publisher, date));
            return false;
        }
    } else if (choice == 1 && tui_confirm("确定删除《%s》吗？", item->name->c_str(item->name))) {
        show_result(admin_delete_book(item));
        return false;
    }
    return false;
}

static bool student_action(void *ctx, size_t row) {
    crud_context *list = ctx;
    student item = list->rows->at(list->rows, row);
    static const char *const options[] = { "1. 修改学生", "2. 删除学生", "3. 返回" };
    tui_menu_spec spec = { .title = "学生操作", .items = options, .count = 3 };
    int choice = tui_menu(&spec);
    if (choice == 0) {
        char name[128], class_name[128], department[128];
        snprintf(name, sizeof(name), "%s", item->name->c_str(item->name));
        snprintf(class_name, sizeof(class_name), "%s", item->class->c_str(item->class));
        snprintf(department, sizeof(department), "%s", item->department->c_str(item->department));
        tui_field fields[] = {
            { "姓名", name, sizeof(name), 0, required },
            { "班级", class_name, sizeof(class_name), 0, required },
            { "学院", department, sizeof(department), 0, required },
        };
        if (tui_form("修改学生", fields, 3, "Tab/↑↓ 切换  Enter 保存  Esc 取消") == TUI_FORM_OK) {
            show_result(admin_update_student(item, name, class_name, department));
            return false;
        }
    } else if (choice == 1 && tui_confirm("确定删除学生 %zu 吗？", item->studentID)) {
        show_result(admin_delete_student(item));
        return false;
    }
    return false;
}

static bool manager_action(void *ctx, size_t row) {
    crud_context *list = ctx;
    manager item = list->rows->at(list->rows, row);
    static const char *const options[] = { "1. 修改姓名", "2. 删除管理员", "3. 返回" };
    tui_menu_spec spec = { .title = "管理员操作", .items = options, .count = 3 };
    int choice = tui_menu(&spec);
    if (choice == 0) {
        if (!verify_current_password(list->current)) return false;
        char name[128];
        snprintf(name, sizeof(name), "%s", item->name->c_str(item->name));
        tui_field field = { "姓名", name, sizeof(name), 0, required };
        if (tui_form("修改管理员", &field, 1, "Enter 保存  Esc 取消") == TUI_FORM_OK) {
            show_result(admin_update_manager(list->current, item, name));
            return false;
        }
    } else if (choice == 1) {
        if (!tui_confirm("确定删除管理员 %zu 吗？", item->id)) return false;
        if (!verify_current_password(list->current)) return false;
        show_result(admin_delete_manager(list->current, item));
        return false;
    }
    return false;
}

static void books_table(manager current) {
    for (;;) {
        crud_context ctx = { admin_list_books(), current };
        static const char *const headers[] = { "ID", "ISBN", "书名", "作者", "状态" };
        static const int widths[] = { 8, 18, 28, 18, 12 };
        tui_table_spec spec = { "图书管理", headers, widths, 5, rows_count, book_row, &ctx, 10,
                                "↑↓/jk 移动  Enter 操作  q 返回", book_action };
        long selected = tui_table(&spec);
        delete_vector(ctx.rows);
        if (selected < 0) return;
    }
}

static void students_table(manager current) {
    for (;;) {
        crud_context ctx = { admin_list_students(), current };
        static const char *const headers[] = { "学号", "姓名", "班级", "学院", "已借" };
        static const int widths[] = { 14, 18, 20, 24, 8 };
        tui_table_spec spec = { "学生管理", headers, widths, 5, rows_count, student_row, &ctx, 10,
                                "↑↓/jk 移动  Enter 操作  q 返回", student_action };
        long selected = tui_table(&spec);
        delete_vector(ctx.rows);
        if (selected < 0) return;
    }
}

static void managers_table(manager current) {
    for (;;) {
        crud_context ctx = { admin_list_managers(), current };
        static const char *const headers[] = { "ID", "姓名", "注册日期", "注册人" };
        static const int widths[] = { 8, 20, 22, 20 };
        tui_table_spec spec = { "管理员管理", headers, widths, 4, rows_count, manager_row, &ctx, 10,
                                "↑↓/jk 移动  Enter 操作  q 返回", manager_action };
        long selected = tui_table(&spec);
        delete_vector(ctx.rows);
        if (selected < 0) return;
    }
}

void tui_admin_add_book(manager current) {
    (void)current;
    char isbn[128] = "", name[128] = "", author[128] = "", publisher[128] = "", date[64] = "";
    tui_field fields[] = {
        { "ISBN", isbn, sizeof(isbn), 0, required }, { "书名", name, sizeof(name), 0, required },
        { "作者", author, sizeof(author), 0, required }, { "出版社", publisher, sizeof(publisher), 0, required },
        { "出版时间", date, sizeof(date), 0, required },
    };
    if (tui_form("增加图书", fields, 5, "Tab/↑↓ 切换  Enter 保存  Esc 取消") == TUI_FORM_OK)
        show_result(admin_add_book(isbn, name, author, publisher, date));
}

void tui_admin_add_student(manager current) {
    (void)current;
    char id[32] = "", name[128] = "", class_name[128] = "", department[128] = "";
    tui_field fields[] = {
        { "学号", id, sizeof(id), TUI_FIELD_NUMERIC, positive_id },
        { "姓名", name, sizeof(name), 0, required }, { "班级", class_name, sizeof(class_name), 0, required },
        { "学院", department, sizeof(department), 0, required },
    };
    if (tui_form("增加学生", fields, 4, "Tab/↑↓ 切换  Enter 保存  Esc 取消") == TUI_FORM_OK)
        show_result(admin_add_student((size_t)strtoumax(id, NULL, 10), name, class_name, department));
}

void tui_admin_add_manager(manager current) {
    char id[32] = "", name[128] = "", password[128] = "", confirmation[128] = "";
    tui_field fields[] = {
        { "管理员 ID", id, sizeof(id), TUI_FIELD_NUMERIC, positive_id },
        { "姓名", name, sizeof(name), 0, required },
        { "密码", password, sizeof(password), TUI_FIELD_MASKED, required },
        { "确认密码", confirmation, sizeof(confirmation), TUI_FIELD_MASKED, required },
    };
    if (tui_form("增加管理员", fields, 4, "Tab/↑↓ 切换  Enter 保存  Esc 取消") == TUI_FORM_OK) {
        if (strcmp(password, confirmation) != 0) {
            tui_error("增加管理员失败", "两次输入的密码不一致");
            return;
        }
        show_result(admin_add_manager(current, (size_t)strtoumax(id, NULL, 10), name, password));
    }
}

void tui_admin_change_password(manager current) {
    if (!verify_current_password(current)) return;
    char password[128] = "", confirmation[128] = "";
    tui_field fields[] = {
        { "新密码", password, sizeof(password), TUI_FIELD_MASKED, required },
        { "确认密码", confirmation, sizeof(confirmation), TUI_FIELD_MASKED, required },
    };
    if (tui_form("修改密码", fields, 2, "Tab/↑↓ 切换  Enter 保存  Esc 取消") == TUI_FORM_OK)
        show_result(admin_change_password(current, current, password, confirmation));
}

void tui_admin_books(manager current) { books_table(current); }
void tui_admin_students(manager current) { students_table(current); }
void tui_admin_managers(manager current) { managers_table(current); }