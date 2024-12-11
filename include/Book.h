#ifndef _book_lib
#define _book_lib

#include"String.h"

typedef struct Book Book;
typedef Book *book;

struct Book{
    // 编号
    int bookId;
    // ISBN出版号
    int ISBN;
    // 借出状态
    int status;
    // 书名
    String bookName;
    // 作者
    String author;
    // 出版商
    String publisher;

    // 表示序列化之后的数据
    char *serialize;

    // 获取图书的数据
    // 返回 const char* 指向序列化的二进制流数据
    const char *(*data)(book);
};

#endif