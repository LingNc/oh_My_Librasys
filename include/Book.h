#ifndef _book_lib
#define _book_lib

#include"String.h"

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

    // 获取图书的数据
    // 返回 const char* 指向序列化的二进制流数据
    const char *(*data)(book);
    // 读入图书的数据，反序列化
    // 返回 int 表示是否正确读入
    int (*in_data)(book,const char*);
};

void book_init(book this){
    memset(this->ISBN,0,sizeof(this->ISBN));
    memset(this->author,0,sizeof(this->author));
    memset(this->bookName,0,sizeof(this->bookName));
    memset(this->publisher,0,sizeof(this->publisher));
    this->status=0;
    this->data = _book_data;
    this->in_data = _book_in_data;
}

#endif