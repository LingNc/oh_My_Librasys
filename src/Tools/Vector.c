#include "Tools/Vector.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// 函数声明
static void _grow(vector this);
static void _init_all(vector this, const char type[]);
static void _vector_push_back(vector this,const void *item);
static void _vector_erase(vector this, size_t position);
static void *_vector_at(vector this, size_t position);
static size_t _vector_size(vector this);
static size_t _vector_find(vector this, const void *key, size_t startIndex);
static void _vector_delete(vector this);
static const char *_vector_data(vector this);
static bool _vector_in_data(vector this, const char *data);
static void _vector_resize(vector this, size_t newSize);
static vector _vector_init_func(vector this);


// 动态扩展向量的容量
static void _grow(vector this){
    this->_allocatedSize*=2;
    this->_data=realloc(this->_data,this->_itemSize*this->_allocatedSize);
    assert(this->_data!=NULL);
}

// 向向量末尾添加元素
static void _vector_push_back(vector this,const void *item){
    if(this->_size==this->_allocatedSize){
        _grow(this);
    }
    void *dest=(char *)this->_data+this->_size*this->_itemSize;
    if(this->_copy_item){
        this->_copy_item(dest,(void *)item);
    }
    else{
        memcpy(dest,item,this->_itemSize);
    }
    this->_size++;
}

// 删除指定位置的元素
static void _vector_erase(vector this,size_t position){
    assert(position<this->_size);
    void *dest=(char *)this->_data+position*this->_itemSize;
    if(this->_free_item){
        this->_free_item(dest);
    }
    memmove(dest,(char *)dest+this->_itemSize,(this->_size-position-1)*this->_itemSize);
    this->_size--;
}

// 获取指定位置的元素
static void *_vector_at(vector this,size_t position){
    if(position>this->size) return NULL;
    return (char *)this->_data+position*this->_itemSize;
}

// 返回当前元素数量
static size_t _vector_size(vector this){
    return this->_size;
}

// 查找元素，返回位置
static size_t _vector_find(vector this,const void *key,size_t startIndex){
    if(startIndex>=this->_size)
        return this->npos;
    for(size_t i=startIndex; i<this->_size; i++){
        void *elem=(char *)this->_data+i*this->_itemSize;
        int cmp_result;
        if(this->_cmp_item){
            cmp_result=this->_cmp_item(elem,(void *)key);
        }
        if(cmp_result==0){
            return i;
        }
    }
    return (size_t)-1;
}

// 释放向量
static void _vector_delete(vector this){
    if(this->_free_item){
        for(size_t i=0; i<this->_size; i++){
            this->_free_item((char *)this->_data+i*this->_itemSize);
        }
    }
    free(this->_data);
    free(this->_base);
    free(this);
}

// 序列化向量数据
static const char *_vector_data(vector this){
    size_t allSize=sizeof(size_t);
    if(this->_size==0){

    }
    char *temp;
    string serlize=this->_serialize;
    // 清空
    serlize->clear(serlize);
    for(size_t i=0;i<=this->_size-1;i++){
        void *pItem=this->at(this,i);
        // 得到每个元素序列化指针
        temp=this->_data_item(pItem);
        // 计算长度
        allSize+=sizeof(size_t)+(size_t)temp;
        // 粘贴到 vector
        serlize->append_n(serlize,temp,(size_t)temp);
    }

}

// 反序列化向量数据
static bool _vector_in_data(vector this,const char *data){
    size_t totalSize=strlen(data);
    size_t newSize=totalSize/this->_itemSize;
    _vector_resize(this,newSize);

    const char *current=data;
    for(size_t i=0; i<newSize; i++){
        void *item=(char *)this->_data+i*this->_itemSize;
        this->_in_data_item(item,current);
        current+=this->_itemSize;
    }
    this->_size=newSize;
    return true;
}

// 调整向量大小
static void _vector_resize(vector this,size_t newSize){
    this->_allocatedSize=newSize;
    this->_data=realloc(this->_data,this->_itemSize*this->_allocatedSize);
    assert(this->_data!=NULL);
}

static vector _vector_init_func(vector this){
    this->push_back=_vector_push_back;
    this->erase=_vector_erase;
    this->at=_vector_at;
    this->size=_vector_size;
    this->find=_vector_find;
    this->free=_vector_delete;
    this->data=_vector_data;
    this->in_data=_vector_in_data;
    this->resize=_vector_resize;
    return this;
}

// 全部初始化
static void _init_all(vector this,const char type[]){
    this->init=_init_all;
    // 初始化成员函数
    _vector_init_func(this);
    // 初始化基本元素类型以及元素函数
    this->_itemSize=_init_type(this,type);
    // 初始化自身变量
    this->npos=(size_t)-1;
    this->_allocatedSize=10;
    this->_serialize=new_string();
    this->_typename=new_string();
    this->_data=malloc(this->_itemSize*this->_allocatedSize);
    assert(this->_data!=NULL);
    this->_size=0;
}

// 创建新的向量
vector new_vector(const char *type){
    vector this=malloc(sizeof(Vector));
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