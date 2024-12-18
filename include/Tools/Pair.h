#ifndef _pair_lib
#define _pair_lib

#include <stdlib.h>
#include "Tools/Type.h"

typedef struct Pair Pair;
typedef Pair *pair;

// 键值对结构体
struct Pair {
    size_t key;
    size_t value;
    _init_default_func(pair);
};

// 创建新的键值对
pair new_pair();
pair __init_Pair();
pair init_pairs(pair this, size_t key, size_t value);

// 释放键值对
void delete_pair(pair this);

#endif