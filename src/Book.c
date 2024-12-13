#include"Book.h"

// data获得序列化数据实现
const char *_book_data(book this){
    memset(this->serialize,0,sizeof(this->serialize));
    size_t len=sizeof(size_t)+sizeof(this->ISBN)+sizeof(this->status)+sizeof(this->author)+sizeof(this->publisher);
    size_t ptr=0;
    memcpy(this->serialize,&len,sizeof(len));
    ptr+=sizeof(len);

    memcpy(this->serialize+ptr,this->ISBN,sizeof(this->ISBN));
    ptr+=sizeof(this->ISBN);

    memcpy(this->serialize+ptr,&this->status,sizeof(this->status));
    ptr+=sizeof(this->status);

    memcpy(this->serialize+ptr,this->author,sizeof(this->author));
    ptr+=sizeof(this->author);

    memcpy(this->serialize+ptr,this->publisher,sizeof(this->publisher));
    ptr+=sizeof(this->publisher);

    return this->serialize;
}

int _book_in_data(book this,const char *data){
    size_t ptr=0;
    size_t len=0;
    memcpy(&len,data+ptr,sizeof(len));
    ptr+=sizeof(len);

    memcpy(this->ISBN,data+ptr,sizeof(this->ISBN));
    ptr+=sizeof(this->ISBN);

    memcpy(&this->status,data+ptr,sizeof(this->status));
    ptr+=sizeof(this->status);

    memcpy(this->author,data+ptr,sizeof(this->author));
    ptr+=sizeof(this->author);

    memcpy(this->publisher,data+ptr,sizeof(this->publisher));
    ptr+=sizeof(this->publisher);

    return 1; // 表示成功
}

// 复制图书数据实现
book _book_copy(book this, book other) {
    memcpy(this->ISBN, other->ISBN, sizeof(this->ISBN));
    this->status = other->status;
    memcpy(this->bookName, other->bookName, sizeof(this->bookName));
    memcpy(this->author, other->author, sizeof(this->author));
    memcpy(this->publisher, other->publisher, sizeof(this->publisher));
    return this;
}

// 比较图书数据实现
int _book_cmp(book this, book other) {
    int result = strcmp(this->ISBN, other->ISBN);
    if (result != 0) return result;
    result = strcmp(this->bookName, other->bookName);
    if (result != 0) return result;
    result = strcmp(this->author, other->author);
    if (result != 0) return result;
    return strcmp(this->publisher, other->publisher);
}

// 释放图书数据实现
void _book_free(book this) {
    // 如果有动态分配的内存，在这里释放
    // 目前没有动态分配的内存，所以不需要做任何事情
}

book new_book(book this){
    memset(this->ISBN,0,sizeof(this->ISBN));
    memset(this->author,0,sizeof(this->author));
    memset(this->bookName,0,sizeof(this->bookName));
    memset(this->publisher,0,sizeof(this->publisher));
    this->status=0;
    this->data = _book_data;
    this->in_data = _book_in_data;
    this->copy = _book_copy;
    this->cmp = _book_cmp;
    this->free = _book_free;
}