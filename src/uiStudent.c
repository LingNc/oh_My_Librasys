#include "uiStudent.h"
#include "Student.h"
#include <stdlib.h>
#include <string.h>

// 函数声明
uistudent new_uistudent();
uistudent new_from_student(student src);
void copy_from_student(uistudent this, student src);

uistudent new_uistudent() {
    uistudent this = (uistudent)malloc(sizeof(uiStudent));
    if (!this) {
        perror("uiStudent: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    this->id = 0;
    this->name = NULL;
    this->class = NULL;
    this->department = NULL;
    this->borrowedCount = 0;
    this->borrowedDate = NULL;
    this->returnDate = NULL;
    this->books = NULL;
    return this;
}

uistudent new_from_student(student src) {
    uistudent this = new_uistudent();
    copy_from_student(this, src);
    return this;
}

void copy_from_student(uistudent this, student src) {
    this->id = src->id;
    this->name = strdup(src->name->c_str(src->name));
    this->class = strdup(src->class->c_str(src->class));
    this->department = strdup(src->department->c_str(src->department));
    this->borrowedCount = src->borrowedCount;
    this->borrowedDate = strdup(src->borrowedDate->c_str(src->borrowedDate));
    this->returnDate = strdup(src->returnDate->c_str(src->returnDate));
    // 需要处理 books 数组的复制
    // ...
}