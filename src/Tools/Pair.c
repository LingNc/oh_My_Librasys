#include "Tools/Pair.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 前置声明所有函数
pair _pair_init(pair this);
pair _pair_copy(pair this, const pair other);
int _pair_cmp(const pair this,const pair other);
pair init_pairs(pair this,size_t key,size_t value);
const char *_pair_data(pair this);
void _pair_in_data(pair this, const char* data);
void free_pair(pair this);

// 创建新的键值对
pair new_pair() {
    pair this = (pair)malloc(sizeof(Pair));
    if (!this) {
        perror("Pair: this 指针分配失败");
        exit(1);
    }
    this->init=_pair_init;
    this->init(this);
    return this;
}
pair init_pairs(pair this, size_t key, size_t value) {
    this->key = key;
    this->value = value;
    return this;
}
// 初始化键值对
pair _pair_init(pair this) {
    this->copy = _pair_copy;
    this->cmp = _pair_cmp;
    this->data = _pair_data;
    this->in_data = _pair_in_data;
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
void free_pair(pair this) {
    free(this);
}

pair __init_Pair(){
    return new_pair();
}