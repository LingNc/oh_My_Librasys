#ifndef _database_lib
#define _database_lib

#include<stdio.h>
#include"Tools/Vector.h"
#include"Tools/String.h"
#include"Tools/Type.h"
#include"Tools/Pair.h"
#include"Student.h"
#include"Book.h"
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
    void (*rm)(dataBase, void *);
    // 保存数据库到文件
    void (*save)(dataBase);
    // 查找数据
    const vector *(*find)(dataBase, const void *);
    // 通过键查找数据
    void *(*find_key)(dataBase, size_t);
    // 清理数据库
    void (*clean)(dataBase);
};

// 加载数据库
dataBase load_database(const char *filePath, const char *type);

// 关闭数据库
void close_database(dataBase this);

// 清理数据库
void clean_database(dataBase this);

#define database(filePath, type) load_database(filePath, #type)
#endif