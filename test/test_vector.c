
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/Tools/Vector.h"

// 整型比较函数
int INT_cmp(const void* elemAddr1, const void* elemAddr2) {
    return *(int*)elemAddr1 - *(int*)elemAddr2;
}

// 浮点数比较函数
int FLOAT_cmp(const void* elemAddr1, const void* elemAddr2) {
    return *(double*)elemAddr1 - *(double*)elemAddr2;
}

// 字符串比较函数
int STR_cmp(const void* elemAddr1, const void* elemAddr2) {
    return strcmp(*(char**)elemAddr1, *(char**)elemAddr2);
}

// 字符串的析构函数
void STR_free(void* elemAddr) {
    free(*(char**)elemAddr);
}

// 字符串的复制函数
void STR_copy(void* destAddr, const void* srcAddr) {
    *(char**)destAddr = strdup(*(char**)srcAddr);
}

void test_int_vector() {
    vector v1 = new_vector("int");
    int Array[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) {
        v1->push_back(v1, &Array[i]);
    }
    assert(v1->size(v1) == 5);
    for (int i = 0; i < 5; i++) {
        assert(*(int*)v1->at(v1, i) == Array[i]);
    }
    v1->clear(v1);
    assert(v1->size(v1) == 0);
    v1->free(v1);
}

void test_double_vector() {
    vector v2 = new_vector("double");
    double ArrayDouble[] = {1.2, 1.3, 3.4, 5.66, 67.8888};
    for (int i = 0; i < 5; i++) {
        v2->push_back(v2, &ArrayDouble[i]);
    }
    assert(v2->size(v2) == 5);
    for (int i = 0; i < 5; i++) {
        assert(*(double*)v2->at(v2, i) == ArrayDouble[i]);
    }
    v2->clear(v2);
    assert(v2->size(v2) == 0);
    v2->free(v2);
}

void test_string_vector() {
    vector vStr = new_vector("char*");
    char* strArray[] = {"hello", "world", "we", "broken", "people", "now"};
    for (int i = 0; i < 6; i++) {
        vStr->push_back(vStr, &strArray[i]);
    }
    assert(vStr->size(vStr) == 6);
    for (int i = 0; i < 6; i++) {
        assert(strcmp(*(char**)vStr->at(vStr, i), strArray[i]) == 0);
    }
    vStr->clear(vStr);
    assert(vStr->size(vStr) == 0);
    vStr->free(vStr);
}

int main() {
    test_int_vector();
    test_double_vector();
    test_string_vector();
    printf("All vector tests passed!\n");
    return 0;
}