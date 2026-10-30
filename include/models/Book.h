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
    int status; // 1: 可借, 0: 已借出
    size_t borrowData;

    // 序列化数据
    string _serialize;
    _init_default_func(book);
};

// 初始化 Book 对象
book __init_Book();
book new_book();

// 释放 Book 对象管理的内存
void load_book(book this,size_t id,const char *ISBN,const char *name,const char *author,const char *publisher,const char *time,int status);

void free_book(book this);

#endif