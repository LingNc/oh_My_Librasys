#include "Tools/Pair.h"
#include <stdio.h>
#include <string.h>

// 前置声明所有函数
pair _pair_init(pair this);
pair _pair_copy(pair this, const pair other);
int _pair_cmp(const pair this, const pair other);
int _pair_dcmp(const pair this, const pair other, size_t itemSize);
const char *_pair_data(pair this);
void _pair_in_data(pair this, const char* data);
void delete_pair(pair this);

// 创建新的键值对
pair new_pair(size_t key, size_t value) {
    pair this = (pair)malloc(sizeof(Pair));
    this->key = key;
    this->value = value;
    this->init = _pair_init;
    this->copy = _pair_copy;
    this->cmp = _pair_cmp;
    this->dcmp = _pair_dcmp;
    this->free = delete_pair;
    this->data = _pair_data;
    this->in_data = _pair_in_data;
    this->init(this);
    return this;
}

// 初始化键值对
pair _pair_init(pair this) {
    // 初始化逻辑
    return this;
}

// 复制键值对
pair _pair_copy(pair this, const pair other) {
    this->key = other->key;
    this->value = other->value;
    return this;
}

// 比较键值对
int _pair_cmp(const pair this, const pair other) {
    if (this->key < other->key) return -1;
    if (this->key > other->key) return 1;
    return 0;
}

// 深度比较键值对
int _pair_dcmp(const pair this, const pair other, size_t itemSize) {
    return memcmp(&this->value, &other->value, itemSize);
}

// 获取键值对数据
const char *_pair_data(pair this) {
    // 返回数据的字符串表示
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "Key: %zu, Value: %zu", this->key, this->value);
    return buffer;
}

// 从数据中初始化键值对
void _pair_in_data(pair this, const char* data) {
    memcpy(&this->key, data, sizeof(size_t));
    memcpy(&this->value, data + sizeof(size_t), sizeof(size_t));
}

// 释放键值对
void delete_pair(pair this) {
    free(this);
}