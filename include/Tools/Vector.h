#ifndef _vector_lib
#define _vector_lib

#include <stdbool.h>
#include <stddef.h>
#include <Tools/String.h>
#include "Type.h"
// #include <Book.h>
// #include <Student.h>

typedef struct Vector Vector;
typedef Vector *vector;

struct Vector
{
    // 无位置常量
    size_t npos;

    // 添加元素到末尾
    // 无返回值
    void (*push_back)(vector this, const void *item);

    // 删除指定位置的元素
    // 无返回值
    void (*remove)(vector this,size_t position);

    // 获取指定位置的元素
    // 返回该元素指针
    void *(*at)(vector this, size_t position);

    // 返回当前元素数量
    // 返回元素数量
    size_t (*size)(vector this);

    // 查找元素，返回位置
    // 返回元素位置
    size_t (*find)(vector this, const void *key, size_t startIndex);

    // 释放向量
    // 无返回值
    void (*free)(vector this);

    // 清空向量
    // 无返回值
    void (*clear)(vector this);

    // 序列化向量数据
    // 返回序列化后的数据指针
    const char *(*data)(vector this);

    // 反序列化向量数据
    // 返回是否成功
    void (*in_data)(vector this, const char *data);

    // 调整向量大小
    // 无返回值
    void (*resize)(vector this, size_t newSize);

    // 初始化
    void (*init)(vector this, const char type[]);
    // 初始化类型
    // void (*iInt)(vector this);
    // void (*iString)(vector this);
    // void (*iBook)(vector this);
    // void (*iStudent)(vector this);

    // 内部数据指针
    void *_data;

    // 内部基准数据函数
    void *_base;
    // 内部数据类型名
    string _typename;

    // 序列化字符串
    string _serialize;

    // 单个元素大小
    size_t _itemSize;

    // 已分配的内存大小
    size_t _allocatedSize;

    // 当前元素数量
    size_t _size;

    // 初始化标准item函数
    _init_default_item(void*,item);
};

// 创建新的向量
vector new_vector(const char *type);

// 对已有的 Vector 初始化
void init_vector(vector this, const char *type);

// 释放向量
void delete_vector(vector this);

#define vector(TYPE) new_vector(#TYPE)

#endif