#include"List.h"
#include"String.h"
#include"Book.h"
#include"Student.h"
#include<string.h>

// 基础类型的cmp函数定义
// 比较两个元素
#define _default_cmp_for(TYPE, a, b) \
    ({ \
        TYPE TYPE_a = *(TYPE *)a; \
        TYPE TYPE_b = *(TYPE *)b; \
        (TYPE_a > TYPE_b) - (TYPE_a < TYPE_b); \
    })

#define _default_cmp_if_for(TYPE) \
    if (itemSize == sizeof(TYPE)) \
        return _default_cmp_for(TYPE, a, b);

int _default_cmp(const void *a, const void *b, size_t itemSize) {
    _default_cmp_if_for(int)
    else _default_cmp_if_for(float)
    else _default_cmp_if_for(double)
    else _default_cmp_if_for(long long)
    else _default_cmp_if_for(long double)
    else _default_cmp_if_for(char)
    else {
        assert(0 && "_default_cmp 传入了不支持的比较类型");
        return 0;
    }
}

// 通用初始化类型函数
#define _init_define_type(this,TYPE) \
    ({ \
        this->_base = malloc(sizeof(TYPE)); \
        this->_init_item=((TYPE *)this->_base)->init; \
        this->_copy_item=((TYPE *)this->_base)->copy; \
        this->_free_item = ((TYPE *)this->_base)->free; \
        this->_cmp_item = ((TYPE *)this->_base)->cmp; \
        this->_data_item = ((TYPE *)this->_base)->data; \
        this->_in_data_item = ((TYPE *)this->_base)->in_data; \
        sizeof(TYPE); \
})

// 简化代码宏替换
#define _init_type_for(TYPE) \
    if (strcmp(type, #TYPE) == 0) \
        result = _init_define_type(this, TYPE);

// 简化代码
#define _init_type_for_basic_with_cmp(TYPE) \
    if (strcmp(type, #TYPE) == 0) { \
        this->_cmp_item = _default_cmp; \
        result=sizeof(TYPE); \
    }


// 对所有支持的数据类型进行对应的匹配，并返回该类型所需的基本大小
#define _init_type(this,type) \
    ({ \
        size_t result = 0; \
        _init_type_for(String); \
        _init_type_for(Book); \
        _init_type_for(Student); \
    else{ \
            this->_copy_item=NULL; \
            this->_free_item=NULL; \
            this->_cmp_item=NULL; \
            _init_type_for_basic_with_cmp(int) \
            _init_type_for_basic_with_cmp(float) \
            _init_type_for_basic_with_cmp(double) \
            _init_type_for_basic_with_cmp(long long) \
            _init_type_for_basic_with_cmp(long double) \
            _init_type_for_basic_with_cmp(char) \
            else{ \
                assert(0&&"_init_type 使用了不受支持的数据类型"); \
                result = 0; \
            } \
        } \
        result; \
    })

#define _init_default_func(type) \
    /* 构造函数 */ \
    type(*init)(type this); \
    /* 拷贝构造函数 */ \
    type(*copy)(type this,book other); \
    /* 比较函数 */ \
    int (*cmp)(type this,book other); \
    /* 析构函数 */ \
    void (*free)(type this); \
    /* 获取图书的序列化数据 */ \
    const char *(*data)(type this); \
    /* 读入图书的数据，反序列化 */ \
    void (*in_data)(type this,const char* data);