#ifndef _uistudent_lib
#define _uistudent_lib

#include <stddef.h>
#include "models/uiBook.h"
#include "models/Student.h"

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

uistudent new_uistudent();
uistudent new_from_student(student src);
void copy_from_student(uistudent this, student src);
uistudent copy_uistudent(uistudent this, uistudent other);
void free_uistudent(uistudent this);
student back_to_student(uistudent this);

#endif