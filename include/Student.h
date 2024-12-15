#ifndef _student_lib
#define _student_lib

#include<string.h>
#include<stddef.h>
#include<stdlib.h>
#include"Book.h"
#include"Tools/Vector.h"

typedef struct Student Student;
typedef Student *student;

struct Student{
    // 姓名
    char name[20];
    // 班级
    char className[20];
    // 借书数量
    int borrowCount;
    // 借阅图书列表，最多10本
    Vector borrowBooks;

    bool (*init)(student this);

    char *_serialize;
    const char *(*data)(student this);
    void (*in_data)(student this,const char *data);
    student(*copy)(student this,student other);
    student(*cmp)(student this,student other);
    student(*free)(student this,student other);
};

#endif