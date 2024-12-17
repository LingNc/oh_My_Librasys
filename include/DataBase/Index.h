#ifndef _index_lib
#define _index_lib

#include "Tools/Vector.h"
#include <stdbool.h>

typedef struct DataBase_Index DataBase_Index;
typedef DataBase_Index *database_index;

// 索引结构体
struct DataBase_Index {
    string filePath;
    size_t (*hash)(size_t key); // 哈希函数
    size_t bucket_count; // 哈希桶数量
    vector *buckets; // 哈希桶数组
};

// 初始化索引
void init_index(database_index index, const char *filePath, size_t bucket_count);

// 关闭索引
void close_index(database_index index);

// 清空索引
void clear_index(database_index index);

// 通过键查找对应的偏移量
size_t find_index(database_index index, size_t key);

// 从文件加载索引
void load_index(database_index index);

// 保存索引到文件
void save_index(database_index index);

// 添加键值对到索引
void add_index(database_index index, size_t key, size_t offset);

// 删除键值对
void remove_index(database_index index, size_t key);

// 重建索引
void rebuild_index(database_index index, const char *filePath);

#endif