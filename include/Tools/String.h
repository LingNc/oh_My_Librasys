#ifndef _string_lib
#define _string_lib

#include <string.h>
#include <stddef.h>
#include <stdbool.h>

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
    // 表示无位置的常量
    size_t npos;
    // 表示序列化之后的数据
    char *_serialize;
    // 获取 C 风格字符串函数
    // 返回 char* 指向该字符串地址
    char *(*c_str)(string);

    // 写入字符
    // 返回写入后的字符串
    string(*write_char)(string,char);

    // 写入 C 风格字符串
    // 返回写入后的字符串
    string(*write_cstr)(string,const char *);

    // 写入 String 对象
    // 返回写入后的字符串
    string(*write)(string,string);

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
    size_t(*find_char)(string,size_t,char);

    // 查找 C 风格字符串
    // 返回字符串位置，未找到返回 npos
    size_t(*find_cstr)(string,size_t,const char *);

    // 查找 String 对象
    // 返回字符串位置，未找到返回 npos
    size_t(*find)(string,size_t,string);

    // 字符串字典序对比
    // 大于返回1，小于返回-1，等于0
    int (*cmp)(string,string);

    // 获取字符串的大小
    // 返回字符串的大小
    size_t(*size)(string);

    // 获取字符串的长度
    // 返回字符串的长度
    size_t(*length)(string);

    // 获取指定位置的字符
    // 返回指���位置的字符
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

    // 获取字符串的原始数据
    // 返回 const char* 指向序列化的二进制流数据
    const char *(*data)(string);

    // 读入序列化数据进行反序列化
    // 返回 true 表示成功，false 表示失败
    bool (*in_data)(string,const char *);

    // 释放指针
    // 返回 true 表示成功，false 表示失败
    bool (*free)(string);
};

// 初始化 String 对象
// 无返回值
string new_string(string this);

// 销毁 String 对象
// 无返回值
void string_destroy(string this);

#endif
