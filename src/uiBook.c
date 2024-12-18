#include "uiBook.h"
#include <stdlib.h>
#include <string.h>

// 函数声明
uibook new_uibook();
uibook new_from_book(book src);
void copy_from_book(uibook this, book src);

uibook new_uibook() {
    uibook this = (uibook)malloc(sizeof(uiBook));
    if (!this) {
        perror("uiBook: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    this->id = 0;
    this->ISBN = NULL;
    this->name = NULL;
    this->author = NULL;
    this->publisher = NULL;
    this->time = NULL;
    this->status = 0;
    return this;
}

uibook new_from_book(book src) {
    uibook this = new_uibook();
    copy_from_book(this, src);
    return this;
}

void copy_from_book(uibook this, book src) {
    this->id = src->id;
    this->ISBN = strdup(src->ISBN->c_str(src->ISBN));
    this->name = strdup(src->name->c_str(src->name));
    this->author = strdup(src->author->c_str(src->author));
    this->publisher = strdup(src->publisher->c_str(src->publisher));
    this->time = strdup(src->time->c_str(src->time));
    this->status = src->status;
}