
#include "uiStudent.h"
#include "Student.h"
#include <stdlib.h>
#include <string.h>

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