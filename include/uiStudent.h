#ifndef _uistudent_lib
#define _uistudent_lib

#include <stddef.h>
#include"uiBook.h"

typedef struct uiStudent uiStudent;
typedef uiStudent *uistudent;
struct uiStudent {
    size_t id;
    char *name;
    char *class;
    char *department;
    int borrowedCount;
    char *borrowedDate;
    char *returnDate;
    // 动态数组，存储借阅的图书信息
    uibook *books;
};

#endif