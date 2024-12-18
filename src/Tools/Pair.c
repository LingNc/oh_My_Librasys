#include "Tools/Pair.h"
#include <stdlib.h>

// 创建新的键值对
pair new_pair(const char *key, const char *valueType) {
    pair this = (pair)malloc(sizeof(Pair));
    this->key = new_string();
    this->key->assign_cstr(this->key, key);
    this->value = new_vector(valueType);
    return this;
}

// 释放键值对
void delete_pair(pair this) {
    delete_string(this->key);
    this->value->clear(this->value);
    free(this->value);
    free(this);
}