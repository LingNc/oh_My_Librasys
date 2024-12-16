#ifndef _student_lib
#define _student_lib

#include<string.h>
#include<stddef.h>
#include<stdlib.h>
#include"Book.h"
#include"Tools/Type.h"
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
    int borrowBooks;

    // 序列化数据
    char *_serialize;
    _init_default_func(student);
};

#endif