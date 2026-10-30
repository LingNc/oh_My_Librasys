#ifndef _pair_lib
#define _pair_lib

#include <stddef.h>
#include "Tools/Type.h"

typedef struct Pair Pair;
typedef Pair *pair;

struct Pair {
    size_t key;
    size_t value;
    _init_default_func(pair);
};

// 初始化 Pair 对象
pair __init_Pair();
pair new_pair();
pair init_pairs(pair this,size_t key,size_t value);
// 释放 Pair 对象本身
void free_pair(pair this);

#endif