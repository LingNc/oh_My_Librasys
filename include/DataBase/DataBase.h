#ifndef _database_lib
#define _database_lib

#include<stdio.h>
#include"Tools/Vector.h"
#include"Tools/String.h"
#include"Tools/Type.h"
#include"DataBase/Index.h"

typedef struct DataBase DataBase;
typedef DataBase *dataBase;

struct DataBase {
    // 文件类型
    string _type;
    // 数据库文件路径
    string filePath;
    // 数据录入缓冲区
    vector _buffer;
    // 临时缓冲区
    vector _temp;
    // 查找结果缓冲区
    vector _find_buffer;
    // 数据库索引
    database_index _index;

    // 添加数据
    void (*add)(dataBase, void *);
    // 删除数据
    void (*rm)(dataBase, size_t);
    // 保存数据库到文件
    void (*save)(dataBase);
    // 查找数据
    Vector* (*find)(dataBase, const void *);
    // 通过键查找数据
    void *(*find_key)(dataBase,size_t);
    // 从_find_buffer 得到索引，从第pos开始，nums个东西
    // 如果为空返回size为0的vector
    Vector *(*get_find)(dataBase,size_t pos,size_t nums);
    // 从数据库根据key 得到索引，从第pos开始，nums个东西
    // 如果为空返回size为0的vector
    Vector *(*get)(dataBase,size_t pos,size_t nums);
    // 获得数据大小
    size_t(*size)(dataBase);
    // 清理数据库
    void (*clean)(dataBase);
};

// 加载数据库
dataBase load_database(const char *filePath, const char *type);

// 关闭数据库
void close_database(dataBase this);

// 清理数据库
void clean_database(dataBase this);

// 获取下一个索引键
size_t get_next_index_key(database_index index);

#define database(filePath, type) load_database(filePath, #type)
#endif