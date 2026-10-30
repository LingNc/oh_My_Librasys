#ifndef _string_lib
#define _string_lib

#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "Type.h"

// String 结构体的前向声明
typedef struct String String;
typedef String *string;

// String 结构体的定义
struct String{
    // 成员变量
    // 指向字符串数据的指针
    char *_data;
    // 字符串的长度
    size_t _length;
    // 已分配内存大小
    size_t _allocatedSize;
    // 表示无位置的常量
    size_t npos;
    // 表示序列化之后的数据
    char *_serialize;
    // 获取 C 风格字符串函数
    // 返回 char* 指向该字符串地址
    char *(*c_str)(string);

    // 写入字符
    // 返回写入后的字符串
    string(*assign_char)(string, char);

    // 写入 C 风格字符串
    // 返回写入后的字符串
    string(*assign_cstr)(string, const char *);

    // 写入 String 对象
    // 返回写String的字符串
    string(*assign)(string, string);

    // 写入字符串的前n个字符
    // 返回写入之后的字符串
    string(*assign_n)(string,const char *,size_t);

    // 删除指定位置的字符
    // 返回删除后的字符串
    string(*erase_char)(string,size_t);

    // 删除第几个位置开始，长度多长的字符串
    // 返回删除后的字符串
    string(*erase)(string,size_t,size_t);

    // 追加字符
    // 返回追加后的字符串
    string(*append_char)(string,char);

    // 追加 C 风格字符串
    // 返回追加后的字符串
    string(*append_cstr)(string,const char *);

    // 追加 String 对象
    // 返回追加后的字符串
    string(*append)(string,string);

    // 追加 char* 指向的字符串的前 n 个字符
    // 返回 追加后的字符串
    string(*append_n)(string, const char *, size_t n);

    // 插入字符
    // 返回插入后的字符串
    string(*insert_char)(string,size_t,char);

    // 插入 C 风格字符串
    // 返回插入后的字符串
    string(*insert_cstr)(string,size_t,const char *);

    // 插入 String 对象
    // 返回插入后的字符串
    string(*insert)(string,size_t,string);

    // 查找字符
    // 返回字符位置，未找到返回 npos
    size_t(*find_char)(string, char, size_t);

    // 查找 C 风格字符串
    // 返回字符串位置，未找到返回 npos
    size_t(*find_cstr)(string, const char *, size_t);

    // 查找 String 对象
    // 返回字符串位置，未找到返回 npos
    size_t(*find)(string, string, size_t);

    // 反向查找字符
    // 返回字符位置，未找到返回 npos
    size_t(*rfind_char)(string, size_t, char);

    // 反向查找 C 风格字符串
    // 返回字符串位置，未找到返回 npos
    size_t(*rfind_cstr)(string, size_t, const char *);

    // 反向查找 String 对象
    // 返回字符串位置，未找到返回 npos
    size_t(*rfind)(string, size_t, string);

    // 替换从pos位置开始长度为len的子字符串为s
    // 返回替换后的字符串
    string(*replace)(string, size_t, size_t, const char *);

    // 交换字符串
    // 无返回值
    void (*swap)(string, string);

    // 追加字符到字符串末尾
    // 无返回值
    void (*push_back)(string, char);

    // 删除字符串末尾的字符
    // 无返回值
    void (*pop_back)(string);

    // 获取字符串的大小
    // 返回字符串的大小
    size_t(*size)(string);

    // 获取字符串的长度
    // 返回字符串的长度
    size_t(*length)(string);

    // 获取指定位置的字符
    // 返回指指定位置的字符
    char (*at)(string,size_t);

    // 获取某个下标位置的指针
    // 返回 char* 指向该位置
    char *(*it)(string,size_t);

    // 清空字符串
    // 无返回值
    void (*clear)(string);

    // 检查字符串是否为空
    // 返回 1 表示为空，0 表示不为空
    int (*empty)(string);

    // 初始化默认函数
    _init_default_func(string);
};

// 新建 String 对象
string new_string();
string __init_String();
// 初始化 String 对象
void init_string(string this);

// 销毁 String 对象
// 无返回值
void delete_string(string this);

#endif
