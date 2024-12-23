#ifndef _student_lib
#define _student_lib

#include "Tools/String.h"
#include "Tools/Vector.h"
#include "models/Book.h" // 添加对 Book.h 的引用

#define MAX_AVAILABLE 10

typedef struct Student Student;
typedef Student *student;

struct Student {
    size_t id;
    size_t studentID;
    string name;
    string class;
    string department;
    int borrowedCount;
    string borrowedDate;
    string returnDate;

    // 序列化数据
    string _serialize;
    _init_default_func(student);
};

void load_student(student this, size_t id, size_t studentID, const char *name, const char *class, const char *department, int borrowedCount, const char *borrowedDate, const char *returnDate);

// 初始化 Student 对象
student __init_Student();
student new_student();

// 释放 Student 对象本身
void free_student(student this);

#endif