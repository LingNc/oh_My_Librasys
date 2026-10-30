#include "service/search.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "models/Book.h"
#include "models/Student.h"

extern dataBase bookDb, studentDb;

/* 大小写不敏感子串匹配（非 ASCII 字节按原样比较，中文可直接命中） */
static bool contains_ci(const char *haystack, const char *needle) {
    if (!haystack || !needle) return false;
    if (!*needle) return false;

    size_t needle_len = strlen(needle);
    for (const char *p = haystack; *p; ++p) {
        size_t i = 0;
        while (i < needle_len && p[i] &&
               tolower((unsigned char)p[i]) == tolower((unsigned char)needle[i])) {
            ++i;
        }
        if (i == needle_len) return true;
    }
    return false;
}

static bool push_key(search_keys *found, size_t *capacity, size_t key) {
    if (found->count == *capacity) {
        size_t next_capacity = *capacity ? *capacity * 2 : 64;
        size_t *next = realloc(found->keys, next_capacity * sizeof(*next));
        if (!next) return false;
        found->keys = next;
        *capacity = next_capacity;
    }
    found->keys[found->count++] = key;
    return true;
}

void search_keys_free(search_keys *found) {
    if (!found) return;
    free(found->keys);
    found->keys = NULL;
    found->count = 0;
    found->truncated = false;
}

search_keys search_books(const char *keyword, size_t limit) {
    search_keys found = { NULL, 0, false };
    if (!keyword || !keyword[0]) return found;

    size_t capacity = 0;
    for (size_t key = 1; key <= bookDb->_index->_last_key; ++key) {
        if (find_index(bookDb->_index, key) == 0) continue;

        book item = bookDb->find_key(bookDb, key);
        if (!item) continue;

        bool hit = contains_ci(item->ISBN->c_str(item->ISBN), keyword) ||
                   contains_ci(item->name->c_str(item->name), keyword) ||
                   contains_ci(item->author->c_str(item->author), keyword) ||
                   contains_ci(item->publisher->c_str(item->publisher), keyword);
        free_book(item);

        if (!hit) continue;
        if (!push_key(&found, &capacity, key)) break;
        if (found.count >= limit) {
            found.truncated = true;
            break;
        }
    }
    return found;
}

search_keys search_students(const char *keyword, size_t limit) {
    search_keys found = { NULL, 0, false };
    if (!keyword || !keyword[0]) return found;

    /* 纯数字关键字额外按学号完全匹配 */
    bool numeric = true;
    for (const char *p = keyword; *p; ++p) {
        if (!isdigit((unsigned char)*p)) { numeric = false; break; }
    }
    size_t keyword_id = numeric ? (size_t)strtoull(keyword, NULL, 10) : 0;

    size_t capacity = 0;
    for (size_t key = 1; key <= studentDb->_index->_last_key; ++key) {
        if (find_index(studentDb->_index, key) == 0) continue;

        student item = studentDb->find_key(studentDb, key);
        if (!item) continue;

        bool hit = (numeric && keyword_id && item->studentID == keyword_id) ||
                   contains_ci(item->name->c_str(item->name), keyword) ||
                   contains_ci(item->class->c_str(item->class), keyword) ||
                   contains_ci(item->department->c_str(item->department), keyword);
        free_student(item);

        if (!hit) continue;
        if (!push_key(&found, &capacity, key)) break;
        if (found.count >= limit) {
            found.truncated = true;
            break;
        }
    }
    return found;
}
