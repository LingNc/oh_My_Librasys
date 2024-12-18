
#include "uiBook.h"
#include <stdlib.h>
#include <string.h>

void copy_from_book(uibook this, book src) {
    this->id = src->id;
    this->ISBN = strdup(src->ISBN->c_str(src->ISBN));
    this->name = strdup(src->name->c_str(src->name));
    this->author = strdup(src->author->c_str(src->author));
    this->publisher = strdup(src->publisher->c_str(src->publisher));
    this->time = strdup(src->time->c_str(src->time));
    this->status = src->status;
}