#include "models/uiStudent.h"
#include "models/Student.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
uistudent new_uistudent();
uistudent new_from_student(student src);
void copy_from_student(uistudent this, student src);
uistudent copy_uistudent(uistudent this, uistudent other);
void free_uistudent(uistudent this);
student back_to_student(uistudent this);

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

void copy_from_student(uistudent this, student src){
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

uistudent copy_uistudent(uistudent this, uistudent other) {
    if (this == other) return this;
    this->id = other->id;
    free(this->name);
    this->name = strdup(other->name);
    free(this->class);
    this->class = strdup(other->class);
    free(this->department);
    this->department = strdup(other->department);
    this->borrowedCount = other->borrowedCount;
    free(this->borrowedDate);
    this->borrowedDate = strdup(other->borrowedDate);
    free(this->returnDate);
    this->returnDate = strdup(other->returnDate);
    // 需要处理 books 数组的深拷贝
    if (this->books) {
        for (int i = 0; i < this->borrowedCount; i++) {
            free_uibook(this->books[i]);
        }
        free(this->books);
    }
    this->books = (uibook *)malloc(other->borrowedCount * sizeof(uibook));
    for (int i = 0; i < other->borrowedCount; i++) {
        this->books[i] = new_uibook();
        copy_uibook(this->books[i], other->books[i]);
    }
    return this;
}

void free_uistudent(uistudent this) {
    if (!this) return;
    free(this->name);
    free(this->class);
    free(this->department);
    free(this->borrowedDate);
    free(this->returnDate);
    if (this->books) {
        for (int i = 0; i < this->borrowedCount; i++) {
            free_uibook(this->books[i]);
        }
        free(this->books);
    }
    // free(this);
}

student back_to_student(uistudent this) {
    student student_new = new_student();
    student_new->id = this->id;
    student_new->name->assign_cstr(student_new->name, this->name);
    student_new->class->assign_cstr(student_new->class, this->class);
    student_new->department->assign_cstr(student_new->department, this->department);
    student_new->borrowedCount = this->borrowedCount;
    student_new->borrowedDate->assign_cstr(student_new->borrowedDate, this->borrowedDate);
    student_new->returnDate->assign_cstr(student_new->returnDate, this->returnDate);
    // 需要处理 books 数组的转换
    // ...
    return student_new;
}