#ifndef _book_lib
#define _book_lib

#include"Tools/String.h"
#include"Tools/Type.h"
typedef struct Book Book;
typedef Book *book;

struct Book{
    // ISBN出版号
    char ISBN[15];
    // 借出状态
    int status;
    // 书名
    char bookName[1000];
    // 作者
    char author[100];
    // 出版商
    char publisher[100];

    // 表示序列化之后的数据
    char serialize[1500];
    _init_default_func(book);
};

void new_book(book this);

#endif