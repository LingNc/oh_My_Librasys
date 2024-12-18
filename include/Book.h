#ifndef _book_lib
#define _book_lib

#include "Tools/String.h"

typedef struct Book Book;
typedef Book *book;

struct Book {
    size_t id;
    string ISBN;
    string name;
    string author;
    string publisher;
    string time;
    int status; // 0: 可借, 1: 已借出

    // 表示序列化之后的数据
    string _serialize;
    _init_default_func(book);
};
book new_book();

// 初始化 Book 对象
book __init_Book();

void load_book(book this, size_t id, const char *ISBN, const char *name, const char *author, const char *publisher, const char *time, int status);

#endif