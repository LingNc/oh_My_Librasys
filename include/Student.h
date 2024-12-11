#ifndef _student_lib
#define _student_lib

#include<string.h>
#include"Book.h"
typedef struct Student Student;
typedef Student *student;

struct Student{
    char name[20]; // 姓名

    char className[20]; // 班级

    int borrowCount; // 借书数量

    Book *borrowBooks[10]; // 借阅图书列表，最多10本
};

#endif