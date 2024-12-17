#ifndef _pair_lib
#define _pair_lib

#include "String.h"
#include "Vector.h"

typedef struct Pair Pair;
typedef Pair *pair;

// 键值对结构体
struct Pair {
    string key;
    vector value;
};

// 创建新的键值对
pair new_pair(const char *key, const char *valueType);

// 释放键值对
void delete_pair(pair this);

#endif