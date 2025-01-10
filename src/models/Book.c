#include "models/Book.h"
#include "Tools/String.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 函数声明
const char *_book_data(book this);
void _book_in_data(book this, const char *data);
book _book_copy(book this, book other);
int _book_cmp(book this, book other);
void _book_free(book this);
book _book_init(book this);
book new_book();
void load_book(book this, size_t id, const char *ISBN, const char *name, const char *author, const char *publisher, const char *time, int status);
void free_book(book this);

// data获得序列化数据实现
const char *_book_data(book this) {
    this->_serialize->clear(this->_serialize);
    size_t len = sizeof(this->id) + sizeof(size_t) + this->ISBN->length(this->ISBN) +
                 sizeof(size_t) + this->name->length(this->name) + sizeof(size_t) + this->author->length(this->author) +
                 sizeof(size_t) + this->publisher->length(this->publisher) + sizeof(size_t) + this->time->length(this->time) +
                 sizeof(this->status) + sizeof(this->borrowData);
    this->_serialize->append_n(this->_serialize, (const char*)&len, sizeof(len));
    this->_serialize->append_n(this->_serialize, (const char*)&this->id, sizeof(this->id));

    size_t str_len = this->ISBN->length(this->ISBN);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->ISBN->c_str(this->ISBN), str_len);

    str_len = this->name->length(this->name);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->name->c_str(this->name), str_len);

    str_len = this->author->length(this->author);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->author->c_str(this->author), str_len);

    str_len = this->publisher->length(this->publisher);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->publisher->c_str(this->publisher), str_len);

    str_len = this->time->length(this->time);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->time->c_str(this->time), str_len);

    this->_serialize->append_n(this->_serialize, (const char*)&this->status, sizeof(this->status));
    this->_serialize->append_n(this->_serialize, (const char*)&this->borrowData, sizeof(this->borrowData));

    return this->_serialize->c_str(this->_serialize);
}

// 反序列化数据实现
void _book_in_data(book this, const char *data) {
    size_t ptr = sizeof(size_t);
    memcpy(&this->id, data + ptr, sizeof(this->id));
    ptr += sizeof(this->id);

    size_t str_len = 0;
    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->ISBN->append_n(this->ISBN, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->name->append_n(this->name, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->author->append_n(this->author, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->publisher->append_n(this->publisher, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->time->append_n(this->time, data + ptr, str_len);
    ptr += str_len;

    memcpy(&this->status, data + ptr, sizeof(this->status));
    ptr += sizeof(this->status);
    memcpy(&this->borrowData, data + ptr, sizeof(this->borrowData));
    ptr += sizeof(this->borrowData);

}

// 复制图书数据实现
book _book_copy(book this,book other){
    this->id=other->id;
    this->ISBN->assign(this->ISBN,other->ISBN);
    this->name->assign(this->name, other->name);
    this->author->assign(this->author, other->author);
    this->publisher->assign(this->publisher, other->publisher);
    this->time->assign(this->time, other->time);
    this->status=other->status;
    this->borrowData=other->borrowData;
    return this;
}

// 比较图书数据实现
int _book_cmp(book this, book other) {
    if (other->ISBN->length(other->ISBN) > 0 && strcmp(this->ISBN->c_str(this->ISBN), other->ISBN->c_str(other->ISBN)) != 0) {
        return 1;
    }
    if (other->name->length(other->name) > 0 && strcmp(this->name->c_str(this->name), other->name->c_str(other->name)) != 0) {
        return 1;
    }
    if (other->author->length(other->author) > 0 && strcmp(this->author->c_str(this->author), other->author->c_str(other->author)) != 0) {
        return 1;
    }
    if (other->publisher->length(other->publisher) > 0 && strcmp(this->publisher->c_str(this->publisher), other->publisher->c_str(other->publisher)) != 0) {
        return 1;
    }
    if (other->time->length(other->time) > 0 && strcmp(this->time->c_str(this->time), other->time->c_str(other->time)) != 0) {
        return 1;
    }
    return 0;
}

// 释放图书数据实现
void _book_free(book this) {
    if (this->ISBN) this->ISBN->free(this->ISBN);
    if (this->name) this->name->free(this->name);
    if (this->author) this->author->free(this->author);
    if (this->publisher) this->publisher->free(this->publisher);
    if (this->time) this->time->free(this->time);
    if (this->_serialize) this->_serialize->free(this->_serialize);
}
// 初始化
book _book_init(book this) {
    this->ISBN = new_string();
    this->name = new_string();
    this->author = new_string();
    this->publisher = new_string();
    this->time = new_string();
    this->_serialize = new_string();
    this->status=0;
    this->borrowData=0;
    this->init=_book_init;
    this->data = _book_data;
    this->in_data = _book_in_data;
    this->copy = _book_copy;
    this->cmp = _book_cmp;
    this->free = _book_free;
    return this;
}
// 初始化图书
book new_book() {
    book this = (book)malloc(sizeof(Book));
    if (!this) {
        perror("Book: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    return _book_init(this);
}

void load_book(book this, size_t id, const char *ISBN, const char *name, const char *author, const char *publisher, const char *time, int status) {
    this->id = id;
    this->ISBN->assign_cstr(this->ISBN, ISBN);
    this->name->assign_cstr(this->name, name);
    this->author->assign_cstr(this->author, author);
    this->publisher->assign_cstr(this->publisher, publisher);
    this->time->assign_cstr(this->time, time);
    this->status = status;
}

// 初始化 Book 对象的函数
book __init_Book() {
    return new_book();
}
// 释放 Book 对象本身
void free_book(book this) {
    if(!this) return;
    this->free(this);
    free(this);
}