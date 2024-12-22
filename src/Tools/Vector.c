#include "Tools/Vector.h"
#include "Tools/Pair.h"
#include "models/Book.h"
#include "models/Student.h"
#include "models/Manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// 函数声明
static void _grow(vector this);
static void _init_all(vector this,const char type[]);
static void _vector_push_back(vector this,const void *item);
static void _vector_remove(vector this,size_t position);
static void _vector_clear(vector this);
static void *_vector_at(vector this,size_t position);
static size_t _vector_size(vector this);
static size_t _vector_find(vector this,const void *key,size_t startIndex);
static void _vector_delete(vector this);
static const char *_vector_data(vector this);
static void _vector_in_data(vector this,const char *data);
static void _vector_resize(vector this,size_t newSize);
static vector _vector_init_func(vector this);


// 动态扩展向量的容量
static void _grow(vector this){
    this->_allocatedSize*=2;
    this->_data=realloc(this->_data,this->_itemSize*this->_allocatedSize);
    if(!this->_data){
        perror("Vector: _data 指针分配失败");
        exit(EXIT_FAILURE);
    }
    assert(this->_data!=NULL);
}

// 向向量末尾添加元素
static void _vector_push_back(vector this,const void *item){
    if(this->_size==this->_allocatedSize){
        _grow(this);
    }
    void *dest=(char *)this->_data+this->_size*this->_itemSize;
    if(this->_init_item){
        this->_init_item(dest);
    }
    if(this->_copy_item){
        this->_copy_item(dest,(void *)item);
    }
    else{
        memcpy(dest,item,this->_itemSize);
    }
    this->_size++;
}

// 删除指定位置的元素
static void _vector_remove(vector this,size_t position){
    assert(position<this->_size);
    void *dest=(char *)this->_data+position*this->_itemSize;
    if(this->_free_item){
        this->_free_item(dest);
    }
    memmove(dest,(char *)dest+this->_itemSize,(this->_size-position-1)*this->_itemSize);
    this->_size--;
}

// 清空向量
static void _vector_clear(vector this) {
    if (this->_free_item) {
        for (size_t i = 0; i < this->_size; i++) {
            this->_free_item(this->_data + i * this->_itemSize);
        }
    }
    this->_data=realloc(this->_data,this->_itemSize*this->_allocatedSize);
    this->_size=0;
}

// 获取指定位置的元素
static void *_vector_at(vector this,size_t position){
    if(position>this->size(this)) return NULL;
    return this->_data+position*(this->_itemSize);
}

// 返回当前元素数量
static size_t _vector_size(vector this){
    return this->_size;
}

// 查找元素，返回位置
static size_t _vector_find(vector this,const void *key,size_t startIndex){
    if(startIndex>=this->_size){
        perror("Vector: _vector_find 下标越界");
        return this->npos;
    }
    for(size_t i=startIndex; i<this->_size; i++){
        void *elem=(char *)this->_data+i*this->_itemSize;
        int cmp_result;
        if(this->_cmp_item){
            cmp_result=this->_cmp_item(elem,(void *)key);
        }
        else{
            cmp_result=this->_dcmp_item(elem,(void *)key,this->_typename->c_str(this->_typename));
        }

        if(cmp_result==0){
            return i;
        }
    }
    return (size_t)-1;
}

// 释放向量
static void _vector_delete(vector this) {
    _vector_clear(this); // 先清空向量
    free(this->_data);
    free(this->_base);
    free(this);
}

// 序列化向量数据
static const char *_vector_data(vector this){
    this->_serialize->clear(this->_serialize);
    size_t totalSize=sizeof(size_t);
    string temp=new_string();

    for(size_t i=0; i<this->_size; ++i){
        void *item=(char *)this->_data+i*this->_itemSize;
        // 获得元素序列化数据
        temp->append_cstr(temp,this->_data_item(item));
    }
    totalSize=temp->size(temp);
    this->_serialize->append_n(this->_serialize,(const char*)&totalSize,sizeof(size_t));
    // 储存有多少个
    this->_serialize->append_n(this->_serialize,(const char*)&this->_size,sizeof(size_t));
    this->_serialize->append(this->_serialize,temp);
    return this->_serialize->c_str(this->_serialize);
}

// 反序列化向量数据
static void _vector_in_data(vector this,const char *data){
    size_t offset=0;
    size_t totalSize=0;
    memcpy(&totalSize,data,sizeof(totalSize));
    offset+=sizeof(totalSize);

    size_t newSize=0;
    memcpy(&newSize,data+offset,sizeof(size_t));
    if(newSize!=0) _vector_resize(this,newSize);

    for(size_t i=0; i<newSize; ++i){
        void *item=(char *)this->_data+i*this->_itemSize;
        memcpy(item,data+offset,this->_itemSize);
        offset+=this->_itemSize;
    }

    // this->_size=newSize;
    // return true;
}

// 调整向量大小
static void _vector_resize(vector this,size_t newSize){
    this->_allocatedSize=newSize;
    this->_data=realloc(this->_data,this->_itemSize*this->_allocatedSize);
    if(!this->_data){
        perror("Vector: _data 指针分配失败");
        exit(EXIT_FAILURE);
    }
    assert(this->_data!=NULL);
}

static vector _vector_init_func(vector this){
    this->push_back=_vector_push_back;
    this->remove=_vector_remove;
    this->at=_vector_at;
    this->size=_vector_size;
    this->find=_vector_find;
    this->free=_vector_delete;
    this->clear=_vector_clear;
    this->data=_vector_data;
    this->in_data=_vector_in_data;
    this->resize=_vector_resize;
    return this;
}

// 全部初始化
static void _init_all(vector this,const char *type){
    this->init=_init_all;

    // 初始化成员函数
    _vector_init_func(this);
    // 初始化基本元素类型以及元素函数
    size_t result = 0;
    if (strcmp(type, "String") == 0) {
        result = ({
            this->_base = __init_String();
            if (!this->_base) {
                perror("Type: _base 指针分配失败");
                exit(1);
            }
            this->_init_item = (void *(*)(void *))((String *)this->_base)->init;
            this->_copy_item = (void *(*)(void *, const void *))((String *)this->_base)->copy;
            this->_free_item = (void (*)(void *))((String *)this->_base)->free;
            this->_cmp_item = (int (*)(const void *, const void *))((String *)this->_base)->cmp;
            this->_data_item = (const char *(*)(void *))((String *)this->_base)->data;
            this->_in_data_item = (void (*)(void *, const char *))((String *)this->_base)->in_data;
            sizeof(String);
        });
    } else if (strcmp(type, "Book") == 0) {
        result = ({
            this->_base = __init_Book();
            if (!this->_base) {
                perror("Type: _base 指针分配失败");
                exit(1);
            }
            this->_init_item = (void *(*)(void *))((Book *)this->_base)->init;
            this->_copy_item = (void *(*)(void *, const void *))((Book *)this->_base)->copy;
            this->_free_item = (void (*)(void *))((Book *)this->_base)->free;
            this->_cmp_item = (int (*)(const void *, const void *))((Book *)this->_base)->cmp;
            this->_data_item = (const char *(*)(void *))((Book *)this->_base)->data;
            this->_in_data_item = (void (*)(void *, const char *))((Book *)this->_base)->in_data;
            sizeof(Book);
        });
    } else if (strcmp(type, "Student") == 0) {
        result = ({
            this->_base = __init_Student();
            if (!this->_base) {
                perror("Type: _base 指针分配失败");
                exit(1);
            }
            this->_init_item = (void *(*)(void *))((Student *)this->_base)->init;
            this->_copy_item = (void *(*)(void *, const void *))((Student *)this->_base)->copy;
            this->_free_item = (void (*)(void *))((Student *)this->_base)->free;
            this->_cmp_item = (int (*)(const void *, const void *))((Student *)this->_base)->cmp;
            this->_data_item = (const char *(*)(void *))((Student *)this->_base)->data;
            this->_in_data_item = (void (*)(void *, const char *))((Student *)this->_base)->in_data;
            sizeof(Student);
        });
    } else if (strcmp(type, "Pair") == 0) {
        result = ({
            this->_base = __init_Pair();
            if (!this->_base) {
                perror("Type: _base 指针分配失败");
                exit(1);
            }
            this->_init_item = (void *(*)(void *))((Pair *)this->_base)->init;
            this->_copy_item = (void *(*)(void *, const void *))((Pair *)this->_base)->copy;
            this->_free_item = (void (*)(void *))((Pair *)this->_base)->free;
            this->_cmp_item = (int (*)(const void *, const void *))((Pair *)this->_base)->cmp;
            this->_data_item = (const char *(*)(void *))((Pair *)this->_base)->data;
            this->_in_data_item = (void (*)(void *, const char *))((Pair *)this->_base)->in_data;
            sizeof(Pair);
        });
    } else if (strcmp(type, "Manager") == 0) {
        result = ({
            this->_base = __init_Manager();
            if (!this->_base) {
                perror("Type: _base 指针分配失败");
                exit(1);
            }
            this->_init_item = (void *(*)(void *))((Manager *)this->_base)->init;
            this->_copy_item = (void *(*)(void *, const void *))((Manager *)this->_base)->copy;
            this->_free_item = (void (*)(void *))((Manager *)this->_base)->free;
            this->_cmp_item = (int (*)(const void *, const void *))((Manager *)this->_base)->cmp;
            this->_data_item = (const char *(*)(void *))((Manager *)this->_base)->data;
            this->_in_data_item = (void (*)(void *, const char *))((Manager *)this->_base)->in_data;
            sizeof(Manager);
        });
    } else {
        this->_copy_item = NULL;
        this->_free_item = NULL;
        this->_cmp_item = NULL;
        this->_init_item = NULL;
        this->_data_item = NULL;
        this->_in_data_item = NULL;
        if (strcmp(type, "int") == 0) {
            this->_dcmp_item = _default_cmp;
            result = sizeof(int);
        } else if (strcmp(type, "float") == 0) {
            this->_dcmp_item = _default_cmp;
            result = sizeof(float);
        } else if (strcmp(type, "double") == 0) {
            this->_dcmp_item = _default_cmp;
            result = sizeof(double);
        } else if (strcmp(type, "size_t") == 0) {
            this->_dcmp_item = _default_cmp;
            result = sizeof(size_t);
        } else if (strcmp(type, "char") == 0) {
            this->_dcmp_item = _default_cmp;
            result = sizeof(char);
        } else {
            result = 0;
        }
    }
    this->_itemSize=result;
    // 初始化自身变量
    this->npos=(size_t)-1;
    this->_allocatedSize=10;
    this->_serialize=new_string();
    this->_typename=new_string();
    this->_typename->append_cstr(this->_typename,type);
    this->_data=malloc(this->_itemSize*this->_allocatedSize);
    if(!this->_data){
        perror("Vector: _data 指针分配失败");
        exit(EXIT_FAILURE);
    }
    assert(this->_data!=NULL);
    this->_size=0;
}

// 创建新的向量
vector new_vector(const char *type){
    vector this=malloc(sizeof(Vector));
    if(!this){
        perror("Vector: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    assert(this!=NULL);
    _init_all(this,type);
    return this;
}

// 对已有的 Vector 初始化
void init_vector(vector this,const char *type){
    _init_all(this,type);
}

// 释放向量
void delete_vector(vector this){
    _vector_delete(this);
}

// 创建 Vector 对象的函数
vector new_Vector() {
    return new_vector("");
}