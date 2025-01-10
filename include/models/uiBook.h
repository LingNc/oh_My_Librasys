#ifndef _uibook_lib
#define _uibook_lib

#include <stddef.h>
#include "models/Book.h"

typedef struct uiBook uiBook;
typedef uiBook *uibook;

struct uiBook {
    size_t id;
    char *ISBN;
    char *name;
    char *author;
    char *publisher;
    char *time;
    int status; // 1: 可借, 0: 已借出
};

uibook new_uibook();
uibook new_from_book(book src);
void copy_from_book(uibook this, book src);
uibook copy_uibook(uibook this, uibook other);
void free_uibook(uibook this);
book uibook_to_book(uibook this);
book back_to_book(uibook this);

#endif