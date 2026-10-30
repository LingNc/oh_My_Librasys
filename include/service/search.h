#ifndef _service_search_h
#define _service_search_h

#include <stdbool.h>
#include <stddef.h>

/*
 * 关键字搜索。
 *
 * 说明：DataBase::find() 目前不会按条件过滤（见文档已知问题 #3），
 * 因此这里按索引逐键读取并做大小写不敏感的子串匹配。
 * 12 万量级全量扫描约 0.6s，达到 limit 后提前结束。
 */

typedef struct {
    size_t *keys;      /* 命中的数据库键，调用方用 search_keys_free 释放 */
    size_t count;
    bool truncated;    /* 达到 limit 仍未扫完，结果不完整 */
} search_keys;

#define SEARCH_DEFAULT_LIMIT 200

/* 图书：ISBN / 书名 / 作者 / 出版社 子串匹配 */
search_keys search_books(const char *keyword, size_t limit);

/* 学生：学号完全匹配，或 姓名 / 班级 / 学院 子串匹配 */
search_keys search_students(const char *keyword, size_t limit);

void search_keys_free(search_keys *found);

#endif // _service_search_h
