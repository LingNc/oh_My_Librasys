#include "Student.h"
#include "Book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
student new_student();
static void borrow_book(student this, book b);
static void return_book(student this, book b);
static const char *_student_data(student this);
static void _student_in_data(student this, const char *data);
void load_student(student this, size_t id, const char *name, const char *class, const char *department, int borrowedCount, const char *borrowedDate, const char *returnDate);

// 初始化学生
student new_student() {
    student this = (student)malloc(sizeof(Student));
    if (!this) {
        perror("Student: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    this->id = 0;
    this->name = new_string();
    this->class = new_string();
    this->department = new_string();
    this->borrowedCount = 0;
    this->borrowedDate = new_string();
    this->returnDate = new_string();
    this->books = new_vector("Book");
    this->data = _student_data;
    this->in_data = _student_in_data;
    return this;
}

// 借书
void borrow_book(student this, book b) {
    if (this->borrowedCount < MAX_AVAILABLE && b->status == 0) {
        this->books->push_back(this->books, b);
        this->borrowedCount++;
        b->status = 1;
        // 更新借书日期
        // ...
    } else {
        printf("无法借书，已达到最大借阅数量或图书已借出。\n");
    }
}

// 还书
void return_book(student this, book b) {
    for (size_t i = 0; i < this->books->size(this->books); ++i) {
        book borrowedBook = (book)this->books->at(this->books, i);
        if (borrowedBook->id == b->id) {
            this->books->remove(this->books, i);
            this->borrowedCount--;
            b->status = 0;
            // 更新还书日期
            // ...
            return;
        }
    }
    printf("未找到借阅的图书。\n");
}

// 序列化学生数据
const char *_student_data(student this) {
    this->_serialize->clear(this->_serialize);
    size_t len = sizeof(this->id) + this->name->length(this->name) + this->class->length(this->class) +
                 this->department->length(this->department) + sizeof(this->borrowedCount) +
                 this->borrowedDate->length(this->borrowedDate) + this->returnDate->length(this->returnDate);
    this->_serialize->append_n(this->_serialize, (const char *)&len, sizeof(len));

    this->_serialize->append_n(this->_serialize, (const char *)&this->id, sizeof(this->id));
    this->_serialize->append_n(this->_serialize, this->name->c_str(this->name), this->name->length(this->name));
    this->_serialize->append_n(this->_serialize, this->class->c_str(this->class), this->class->length(this->class));
    this->_serialize->append_n(this->_serialize, this->department->c_str(this->department), this->department->length(this->department));
    this->_serialize->append_n(this->_serialize, (const char *)&this->borrowedCount, sizeof(this->borrowedCount));
    this->_serialize->append_n(this->_serialize, this->borrowedDate->c_str(this->borrowedDate), this->borrowedDate->length(this->borrowedDate));
    this->_serialize->append_n(this->_serialize, this->returnDate->c_str(this->returnDate), this->returnDate->length(this->returnDate));

    return this->_serialize->c_str(this->_serialize);
}

// 反序列化学生数据
void _student_in_data(student this, const char *data) {
    size_t offset = 0;
    size_t len = 0;
    memcpy(&len, data + offset, sizeof(len));
    offset += sizeof(len);

    memcpy(&this->id, data + offset, sizeof(this->id));
    offset += sizeof(this->id);
    this->name->assign_cstr(this->name, data + offset);
    offset += this->name->length(this->name);
    this->class->assign_cstr(this->class, data + offset);
    offset += this->class->length(this->class);
    this->department->assign_cstr(this->department, data + offset);
    offset += this->department->length(this->department);
    memcpy(&this->borrowedCount, data + offset, sizeof(this->borrowedCount));
    offset += sizeof(this->borrowedCount);
    this->borrowedDate->assign_cstr(this->borrowedDate, data + offset);
    offset += this->borrowedDate->length(this->borrowedDate);
    this->returnDate->assign_cstr(this->returnDate, data + offset);
    offset += this->returnDate->length(this->returnDate);
}

void load_student(student this, size_t id, const char *name, const char *class, const char *department, int borrowedCount, const char *borrowedDate, const char *returnDate) {
    this->id = id;
    this->name->assign_cstr(this->name, name);
    this->class->assign_cstr(this->class, class);
    this->department->assign_cstr(this->department, department);
    this->borrowedCount = borrowedCount;
    this->borrowedDate->assign_cstr(this->borrowedDate, borrowedDate);
    this->returnDate->assign_cstr(this->returnDate, returnDate);
}

// 初始化 Student 对象的函数
student __init_Student() {
    return new_student();
}
