#include "uiBook.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 函数声明
uibook new_uibook();
uibook new_from_book(book src);
void copy_from_book(uibook this, book src);
uibook copy_uibook(uibook this, uibook other);
void free_uibook(uibook this);
book back_to_book(uibook this);

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

uibook copy_uibook(uibook this, uibook other) {
    if (this == other) return this;
    this->id = other->id;
    free(this->ISBN);
    this->ISBN = strdup(other->ISBN);
    free(this->name);
    this->name = strdup(other->name);
    free(this->author);
    this->author = strdup(other->author);
    free(this->publisher);
    this->publisher = strdup(other->publisher);
    free(this->time);
    this->time = strdup(other->time);
    this->status = other->status;
    return this;
}

void free_uibook(uibook this) {
    if (!this) return;
    free(this->ISBN);
    free(this->name);
    free(this->author);
    free(this->publisher);
    free(this->time);
    // free(this);
}

book back_to_book(uibook this) {
    book book_new = new_book();
    book_new->id = this->id;
    book_new->ISBN->assign_cstr(book_new->ISBN, this->ISBN);
    book_new->name->assign_cstr(book_new->name, this->name);
    book_new->author->assign_cstr(book_new->author, this->author);
    book_new->publisher->assign_cstr(book_new->publisher, this->publisher);
    book_new->time->assign_cstr(book_new->time, this->time);
    book_new->status = this->status;
    return book_new;
}