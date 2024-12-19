#include "Student.h"
#include "Book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
student new_student();
static void borrow_book(student this,book b);
static void return_book(student this,book b);
static student _student_init(student this);
static const char *_student_data(student this);
static void _student_free(student this);
static void _student_in_data(student this,const char *data);
void load_student(student this,size_t id,const char *name,const char *class,const char *department,int borrowedCount,const char *borrowedDate,const char *returnDate);
void free_student(student this);
static void _student_copy(student this, student other);
static int _student_cmp(student this, student other);

student _student_init(student this){
    this->id=0;
    this->name=new_string();
    this->class=new_string();
    this->department=new_string();
    this->borrowedCount=0;
    this->borrowedDate=new_string();
    this->returnDate=new_string();
    this->_serialize=new_string();
    this->books=new_vector("Book");
    this->data=_student_data;
    this->in_data=_student_in_data;
    this->init=_student_init;
    this->copy=_student_copy;
    this->cmp=_student_cmp;
    this->free=_student_free;
    return this;
}

// 初始化学生
student new_student(){
    student this=(student)malloc(sizeof(Student));
    if(!this){
        perror("Student: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    _student_init(this);
    return this;
}

// 借书
void borrow_book(student this,book b){
    if(this->borrowedCount<MAX_AVAILABLE&&b->status==0){
        this->books->push_back(this->books,b);
        this->borrowedCount++;
        b->status=1;
        // 更新借书日期
        // ...
    }
    else{
        printf("无法借书，已达到最大借阅数量或图书已借出。\n");
    }
}

// 还书
void return_book(student this,book b){
    for(size_t i=0; i<this->books->size(this->books); ++i){
        book borrowedBook=(book)this->books->at(this->books,i);
        if(borrowedBook->id==b->id){
            this->books->remove(this->books,i);
            this->borrowedCount--;
            b->status=0;
            // 更新还书日期
            // ...
            return;
        }
    }
    printf("未找到借阅的图书。\n");
}

// 序列化学生数据
const char *_student_data(student this){
    this->_serialize->clear(this->_serialize);
    size_t len = sizeof(size_t) + sizeof(this->id) + sizeof(size_t) + this->name->length(this->name) +
                 sizeof(size_t) + this->class->length(this->class) + sizeof(size_t) + this->department->length(this->department) +
                 sizeof(this->borrowedCount) + sizeof(size_t) + this->borrowedDate->length(this->borrowedDate) +
                 sizeof(size_t) + this->returnDate->length(this->returnDate);
    this->_serialize->append_n(this->_serialize, (const char*)&len, sizeof(len));

    this->_serialize->append_n(this->_serialize, (const char*)&this->id, sizeof(this->id));

    size_t str_len = this->name->length(this->name);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->name->c_str(this->name), str_len);

    str_len = this->class->length(this->class);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->class->c_str(this->class), str_len);

    str_len = this->department->length(this->department);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->department->c_str(this->department), str_len);

    this->_serialize->append_n(this->_serialize, (const char*)&this->borrowedCount, sizeof(this->borrowedCount));

    str_len = this->borrowedDate->length(this->borrowedDate);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->borrowedDate->c_str(this->borrowedDate), str_len);

    str_len = this->returnDate->length(this->returnDate);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->returnDate->c_str(this->returnDate), str_len);

    return this->_serialize->c_str(this->_serialize);
}

// 反序列化学生数据
void _student_in_data(student this, const char *data) {
    size_t ptr = 0;
    size_t len = 0;
    memcpy(&this->id, data + ptr, sizeof(this->id));
    ptr += sizeof(this->id);

    size_t str_len = 0;
    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->name->append_n(this->name, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->class->append_n(this->class, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->department->append_n(this->department, data + ptr, str_len);
    ptr += str_len;

    memcpy(&this->borrowedCount, data + ptr, sizeof(this->borrowedCount));
    ptr += sizeof(this->borrowedCount);

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->borrowedDate->append_n(this->borrowedDate, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->returnDate->append_n(this->returnDate, data + ptr, str_len);
    ptr += str_len;
}

void load_student(student this,size_t id,const char *name,const char *class,const char *department,int borrowedCount,const char *borrowedDate,const char *returnDate){
    this->id=id;
    this->name->assign_cstr(this->name,name);
    this->class->assign_cstr(this->class,class);
    this->department->assign_cstr(this->department,department);
    this->borrowedCount=borrowedCount;
    this->borrowedDate->assign_cstr(this->borrowedDate,borrowedDate);
    this->returnDate->assign_cstr(this->returnDate,returnDate);
}

// 初始化 Student 对象的函数
student __init_Student(){
    return new_student();
}

// 释放 Student 对象管理的内存
void _student_free(student this){
    delete_string(this->name);
    delete_string(this->class);
    delete_string(this->department);
    delete_string(this->borrowedDate);
    delete_string(this->returnDate);
    if(this->books){
        // for(size_t i=0; i<this->borrowedCount; ++i){
            // ((book)this->books->at(this->books,i))->free;
        // }
        // this->books->free(this->books);
    }
    delete_string(this->_serialize);
}

// 释放 Student 对象本身
void free_student(student this){
    if(!this) return;
    this->free(this);
    free(this);
}

static void _student_copy(student this, student other) {
    this->id = other->id;
    this->name->assign_cstr(this->name, other->name->c_str(other->name));
    this->class->assign_cstr(this->class, other->class->c_str(other->class));
    this->department->assign_cstr(this->department, other->department->c_str(other->department));
    this->borrowedCount = other->borrowedCount;
    this->borrowedDate->assign_cstr(this->borrowedDate, other->borrowedDate->c_str(other->borrowedDate));
    this->returnDate->assign_cstr(this->returnDate, other->returnDate->c_str(other->returnDate));
    this->books->clear(this->books);
    for (size_t i = 0; i < other->books->size(other->books); ++i) {
        book b = (book)other->books->at(other->books, i);
        this->books->push_back(this->books, b);
    }
}

int _student_cmp(student this, student other) {
    if (this->id != other->id) return this->id - other->id;
    int name_cmp = strcmp(this->name->c_str(this->name), other->name->c_str(other->name));
    if (name_cmp != 0) return name_cmp;
    int class_cmp = strcmp(this->class->c_str(this->class), other->class->c_str(other->class));
    if (class_cmp != 0) return class_cmp;
    int department_cmp = strcmp(this->department->c_str(this->department), other->department->c_str(other->department));
    if (department_cmp != 0) return department_cmp;
    if (this->borrowedCount != other->borrowedCount) return this->borrowedCount - other->borrowedCount;
    int borrowedDate_cmp = strcmp(this->borrowedDate->c_str(this->borrowedDate), other->borrowedDate->c_str(other->borrowedDate));
    if (borrowedDate_cmp != 0) return borrowedDate_cmp;
    return strcmp(this->returnDate->c_str(this->returnDate), other->returnDate->c_str(other->returnDate));
}
