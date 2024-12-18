#ifndef _uibook_lib
#define _uibook_lib

#include<stddef.h>


typedef struct uiBook uiBook;
typedef uiBook *uibook;
struct uiBook {
    size_t id;
    char *ISBN;
    char *name;
    char *author;
    char *publisher;
    char *time;
    int status; // 0: 可借, 1: 已借出
};

#endif