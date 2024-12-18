#ifndef _type_lib
#define _type_lib

#include<string.h>
#include<assert.h>
/*
定义：
    基础类型的cmp函数定义
用途：
    比较两个元素，可以按照指定类型进行比较，用于基础的数据类型进行比较
格式：
    _default_cmp_for(TYPE,a,b)，TYPE为指定的类型，a，b为要比较的两个变量
*/
#define _default_cmp_for(TYPE, a, b) \
    ({ \
        TYPE TYPE_a = *(TYPE *)a; \
        TYPE TYPE_b = *(TYPE *)b; \
        (TYPE_a > TYPE_b) - (TYPE_a < TYPE_b); \
    })

/*
定义：
    根据元素大小选择合适的比较函数
用途：
    根据元素的大小选择合适的比较函数，用于比较不同大小的元素
格式：
    _default_cmp_if_for(TYPE)，TYPE为指定的类型
*/
#define _default_cmp_if_for(TYPE) \
    if (itemSize == sizeof(TYPE)) \
        return _default_cmp_for(TYPE, a, b);

/*
定义：
    默认比较函数，非自定义结构体的比较函数
用途：
    根据元素大小选择合适的比较函数，用于比较不同大小的元素
格式：
    _default_cmp(const void *a,const void *b,size_t itemSize)，a，b为要比较的两个变量，itemSize为元素大小
实现：
    在TYPE.c文件中
*/
extern int _default_cmp(const void *a,const void *b,size_t itemSize);

/*
定义：
    通用初始化类型函数
用途：
    初始化类型的各个函数指针，用于初始化不同类型的函数指针
格式：
    _init_define_type(this,TYPE)，this为当前对象，TYPE为指定的类型
*/
#define _init_define_type(this,TYPE) \
    ({ \
        this->_base = malloc(sizeof(TYPE)); \
        if (!this->_base) { \
            perror("Type: _base 指针分配失败"); \
            exit(EXIT_FAILURE); \
        } \
        this->_init_item = (void *(*)(void *))((TYPE *)this->_base)->init; \
        this->_copy_item = (void *(*)(void *,  const void *))((TYPE *)this->_base)->copy; \
        this->_free_item = (void (*)(void *))((TYPE *)this->_base)->free; \
        this->_cmp_item = (int (*)(const void *, const void *))((TYPE *)this->_base)->cmp; \
        this->_data_item = (const char *(*)(void *))((TYPE *)this->_base)->data; \
        this->_in_data_item = (void (*)(void *, const char *))((TYPE *)this->_base)->in_data; \
        sizeof(TYPE); \
    })

/*
定义：
    简化代码宏替换
用途：
    根据类型名称初始化类型，用于简化代码
格式：
    _init_type_for(TYPE)，TYPE为指定的类型
*/
#define _init_type_for(TYPE) \
    if (strcmp(type, #TYPE) == 0) \
        result = _init_define_type(this, TYPE);

/*
定义：
    简化代码
用途：
    初始化基本类型的比较函数，用于简化代码
格式：
    _init_type_for_basic_with_cmp(TYPE)，TYPE为指定的类型
*/
#define _init_type_for_basic_with_cmp(TYPE) \
    if (strcmp(type, #TYPE) == 0) { \
        this->_dcmp_item = _default_cmp; \
        result=sizeof(TYPE); \
    }

/*
定义：
    对所有支持的数据类型进行对应的匹配，并返回该类型所需的基本大小
用途：
    初始化类型的各个函数指针，用于初始化不同类型的函数指针
格式：
    _init_type(this,type)，this为当前对象，type为指定的类型
*/
#define _init_type(this,type) \
    ({ \
        size_t result = 0; \
        _init_type_for(String) \
        _init_type_for(Book) \
        _init_type_for(Student) \
        _init_type_for(Pair) \
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

/*
定义：
    为自定义的每一种类型(结构体)，快速的初始化默认函数指针
用途：
    定义类型的默认函数指针，用于初始化不同类型的函数指针
格式：
    _init_default_func(type)，type为指定的类型
*/
#define _init_default_func(type) \
    /* 构造函数 */ \
    type (*init)(type this); \
    /* 拷贝构造函数 */ \
    type (*copy)(type this,const type other); \
    /* 比较函数 */ \
    int (*cmp)(const type this,const type other); \
    /* 标准比较函数*/ \
    int (*dcmp)(const type this,const type other,size_t itemSize); \
/* 析构函数 */ \
    void (*free)(type this); \
    /* 获取图书的序列化数据 */ \
    const char *(*data)(type this); \
    /* 读入图书的数据，反序列化 */ \
    void (*in_data)(type this,const char* data);

/*
定义：
    为自定义的每一种类型(结构体)，快速的初始化默认函数指针
用途：
    定义类型的默认函数指针，用于初始化不同类型的函数指针
格式：
    _init_default_(type,item)，type为指定的类型，item为指定的项目
*/
#define _init_default_item(type,item) \
    /* item构造函数 */ \
    type (*_init_##item)(type this); \
    /* item拷贝构造函数 */ \
    type (*_copy_##item)(type this,const type other); \
    /* item标准比较函数 */ \
    int (*_dcmp_##item)(const type this,const type other,size_t itemSize); \
    /* item比较函数 */ \
    int (*_cmp_##item)(const type this,const type other); \
    /* item析构函数 */ \
    void (*_free_##item)(type this); \
    /* item获取图书的序列化数据 */ \
    const char *(*_data_##item)(type this); \
    /* item读入图书的数据，反序列化 */ \
    void (*_in_data_##item)(type this,const char* data);

// // 创建新的数据项
// void *new_item(const char *type);

// 定义类型初始化宏
#define _init_type_for_custom(TYPE) \
    if (strcmp(type, #TYPE) == 0) { \
        item = malloc(sizeof(TYPE)); \
        if (!item) { \
            perror("Type: item 指针分配失败"); \
            exit(EXIT_FAILURE); \
        } \
        ((TYPE *)item)->init((TYPE *)item); \
    }

#endif