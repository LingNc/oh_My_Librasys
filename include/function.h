#ifndef _function_lib
#define _function_lib

#include "models/uiBook.h"
#include "models/uiStudent.h"
#include "DataBase/DataBase.h"

// 加载书籍数据到数据库
bool load_books_from_file(const char *filename, dataBase bookDb);

// 加载学生数据到数据库
bool load_students_from_file(const char *filename, dataBase studentDb);

// 加载学生借阅记录
vector load_borrow_records(dataBase borrowDb, size_t student_id);

// 保存学生借阅记录
void save_borrow_records(dataBase borrowDb, size_t student_id, vector records);

// 初始化root账户
void init_root();

// 创建文件夹
void create_folder(const char *path);

#endif