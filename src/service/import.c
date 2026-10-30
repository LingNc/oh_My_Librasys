#include "service/import.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "Tools/Vector.h"
#include "models/Book.h"
#include "models/Student.h"

extern dataBase bookDb, studentDb;
extern database_index stoid;

#define IMPORT_LINE_CAP 2048
#define BOOK_ISBN_CAP 100
#define BOOK_NAME_CAP 50
#define BOOK_AUTHOR_CAP 500
#define BOOK_PUBLISHER_CAP 500
#define BOOK_TIME_CAP 20
#define STUDENT_FIELD_CAP 50
#define IMPORT_SAVE_BATCH 4096

/*
 * DataBase 的 _database_add() 里更新 _last_key 的那行被注释掉了，
 * 而 get_new_key() 依赖 _last_key 自增。若连续 add 而不 save，
 * 所有记录会拿到同一个键，索引里只剩一条可查。
 *
 * CLI 旧导入是每行 save 一次，所以不会暴露该问题。
 * 这里在 add 之后同步推进 _last_key，并定期 save，
 * 从而在不修改 src/DataBase/ 的前提下得到唯一键。
 */
static void add_unique(dataBase db, void *item) {
    db->add(db, item);
    size_t key = *(size_t *)item;
    if (key > db->_index->_last_key) {
        db->_index->_last_key = key;
    }
}

static import_result make_result(bool ok, size_t imported, size_t line,
                                 const char *message) {
    import_result result = { ok, imported, line, message };
    return result;
}

static size_t count_lines(FILE *file) {
    char line[IMPORT_LINE_CAP];
    size_t count = 0;
    rewind(file);
    while (fgets(line, sizeof(line), file)) ++count;
    rewind(file);
    return count;
}

static bool split_fields(char *line, char **fields, size_t expected) {
    size_t count = 1;
    fields[0] = line;
    for (char *p = line; *p; ++p) {
        if (*p == '\r' || *p == '\n') {
            *p = '\0';
            break;
        }
        if (*p == ',') {
            *p = '\0';
            if (count == expected) return false;
            fields[count++] = p + 1;
        }
    }
    return count == expected;
}

static bool field_fits(const char *field, size_t cap, bool allow_empty) {
    size_t length = strlen(field);
    return (allow_empty || length > 0) && length < cap;
}

static int compare_size_t(const void *left, const void *right) {
    size_t a = *(const size_t *)left;
    size_t b = *(const size_t *)right;
    return (a > b) - (a < b);
}

static bool parse_book_line(char *line, char **fields) {
    return split_fields(line, fields, 5) &&
           field_fits(fields[0], BOOK_ISBN_CAP, false) &&
           field_fits(fields[1], BOOK_NAME_CAP, false) &&
           field_fits(fields[2], BOOK_AUTHOR_CAP, false) &&
           field_fits(fields[3], BOOK_PUBLISHER_CAP, false) &&
           field_fits(fields[4], BOOK_TIME_CAP, true);
}

static bool parse_student_line(char *line, char **fields, size_t *student_id) {
    if (!split_fields(line, fields, 4) ||
        !field_fits(fields[0], 32, false) ||
        !field_fits(fields[1], STUDENT_FIELD_CAP, false) ||
        !field_fits(fields[2], STUDENT_FIELD_CAP, false) ||
        !field_fits(fields[3], STUDENT_FIELD_CAP, false)) {
        return false;
    }

    errno = 0;
    char *end = NULL;
    unsigned long long value = strtoull(fields[0], &end, 10);
    if (errno || !end || *end != '\0' || value == 0 ||
        value > (unsigned long long)(size_t)-1) {
        return false;
    }
    *student_id = (size_t)value;
    return true;
}

import_result import_books_file(const char *path, import_progress_fn progress,
                                void *context) {
    FILE *file = fopen(path, "r");
    if (!file) return make_result(false, 0, 0, "无法打开图书文件");

    size_t total = count_lines(file);
    char line[IMPORT_LINE_CAP];
    size_t line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        ++line_number;
        char *fields[5];
        if (!parse_book_line(line, fields)) {
            fclose(file);
            return make_result(false, 0, line_number,
                               "图书行格式错误或字段超长（需要 5 列）");
        }
        if (progress && (line_number == total || line_number % 128 == 0)) {
            progress("校验图书文件", line_number, total, context);
        }
    }

    if (ferror(file)) {
        fclose(file);
        return make_result(false, 0, line_number, "读取图书文件失败");
    }

    rewind(file);
    line_number = 0;
    size_t imported = 0;
    while (fgets(line, sizeof(line), file)) {
        ++line_number;
        char *fields[5];
        if (!parse_book_line(line, fields)) {
            fclose(file);
            return make_result(false, imported, line_number,
                               "图书文件在校验后发生变化，已停止导入");
        }
        book item = new_book();
        load_book(item, 0, fields[0], fields[1], fields[2], fields[3], fields[4], 0);
        add_unique(bookDb, item);
        free_book(item);
        ++imported;
        if (imported % IMPORT_SAVE_BATCH == 0) {
            bookDb->save(bookDb);
        }
        if (progress && (imported == total || imported % 128 == 0)) {
            progress("导入图书", imported, total, context);
        }
    }
    fclose(file);
    bookDb->save(bookDb);
    return make_result(true, imported, 0, "图书导入完成");
}

import_result import_students_file(const char *path, import_progress_fn progress,
                                   void *context) {
    FILE *file = fopen(path, "r");
    if (!file) return make_result(false, 0, 0, "无法打开学生文件");

    size_t total = count_lines(file);
    size_t *student_ids = total ? malloc(sizeof(*student_ids) * total) : NULL;
    if (total && !student_ids) {
        fclose(file);
        return make_result(false, 0, 0, "内存不足");
    }

    char line[IMPORT_LINE_CAP];
    size_t line_number = 0;
    while (fgets(line, sizeof(line), file)) {
        ++line_number;
        char *fields[4];
        size_t student_id = 0;
        if (!parse_student_line(line, fields, &student_id)) {
            free(student_ids);
            fclose(file);
            return make_result(false, 0, line_number,
                               "学生行格式错误或字段无效（需要 4 列和正整数学号）");
        }

        student_ids[line_number - 1] = student_id;
        if (progress && (line_number == total || line_number % 128 == 0)) {
            progress("校验学生文件", line_number, total, context);
        }
    }

    if (ferror(file)) {
        free(student_ids);
        fclose(file);
        return make_result(false, 0, line_number, "读取学生文件失败");
    }
    qsort(student_ids, total, sizeof(*student_ids), compare_size_t);
    for (size_t i = 0; i < total; ++i) {
        if ((i > 0 && student_ids[i] == student_ids[i - 1]) ||
            find_index(stoid, student_ids[i]) != 0) {
            free(student_ids);
            fclose(file);
            return make_result(false, 0, 0, "学生学号重复或已存在");
        }
    }
    free(student_ids);

    rewind(file);
    line_number = 0;
    size_t imported = 0;
    while (fgets(line, sizeof(line), file)) {
        ++line_number;
        char *fields[4];
        size_t student_id = 0;
        if (!parse_student_line(line, fields, &student_id)) {
            fclose(file);
            return make_result(false, imported, line_number,
                               "学生文件在校验后发生变化，已停止导入");
        }
        student item = new_student();
        load_student(item, 0, student_id, fields[1], fields[2], fields[3], 0, "", "");
        add_unique(studentDb, item);
        add_index(stoid, item->studentID, item->id);
        free_student(item);
        ++imported;
        if (imported % IMPORT_SAVE_BATCH == 0) {
            studentDb->save(studentDb);
            save_index(stoid);
        }
        if (progress && (imported == total || imported % 128 == 0)) {
            progress("导入学生", imported, total, context);
        }
    }
    fclose(file);
    studentDb->save(studentDb);
    save_index(stoid);
    return make_result(true, imported, 0, "学生导入完成");
}