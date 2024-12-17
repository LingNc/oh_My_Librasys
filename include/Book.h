#ifndef _book_lib
#define _book_lib

#include "Tools/String.h"
#include "Tools/Type.h"

#define ISBN_WIDTH 15
#define BOOKNAME_WIDTH 1000
#define AUTHOR_WIDTH 100
#define PUBLISHER_WIDTH 100

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

#endif