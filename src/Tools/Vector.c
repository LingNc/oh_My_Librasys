#include "Tools/Vector.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

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

// 比较两个元素
static int _default_cmp(const void *a, const void *b, size_t itemSize) {
    if (itemSize == sizeof(int)) {
        int int_a = *(int *)a;
        int int_b = *(int *)b;
        return (int_a > int_b) - (int_a < int_b);
    } else if (itemSize == sizeof(float)) {
        float float_a = *(float *)a;
        float float_b = *(float *)b;
        return (float_a > float_b) - (float_a < float_b);
    } else if (itemSize == sizeof(double)) {
        double double_a = *(double *)a;
        double double_b = *(double *)b;
        return (double_a > double_b) - (double_a < double_b);
    } else if (itemSize == sizeof(long long)) {
        long long ll_a = *(long long *)a;
        long long ll_b = *(long long *)b;
        return (ll_a > ll_b) - (ll_a < ll_b);
    } else if (itemSize == sizeof(long double)) {
        long double ld_a = *(long double *)a;
        long double ld_b = *(long double *)b;
        return (ld_a > ld_b) - (ld_a < ld_b);
    } else if (itemSize == sizeof(char)) {
        char char_a = *(char *)a;
        char char_b = *(char *)b;
        return (char_a > char_b) - (char_a < char_b);
    }
    assert(0 && "Vector: _default_cmp 传入了不支持的比较类型");
    return 0;
}

// 查找元素，返回位置
static size_t _vector_find(vector this,const void *key,size_t startIndex){
    if(startIndex>=this->_size)
        return this->npos;
    for(size_t i=startIndex; i<this->_size; i++){
        void *elem=(char *)this->_data+i*this->_itemSize;
        int cmp_result;
        if (this->_cmp_item) {
            cmp_result = this->_cmp_item(elem, (void *)key);
        } else {
            cmp_result = _default_cmp(elem, key, this->_itemSize);
        }
        if(cmp_result == 0){
            return i;
        }
    }
    return (size_t)-1;
}

// 释放向量
static void _vector_free(vector this){
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

// 此处注释目前不删除 未来也许会用

// // 比较两个整数
// static int _int_cmp(int a,int b){
//     if(a>b) return 1;
//     else if(a<b) return -1;
//     else return 0;
// }

// // 初始化整数类型向量
// static void _vector_init_int(vector this){
//     _init_size(this,sizeof(int));
//     this->_copy_item=NULL;
//     this->_free_item=NULL;
//     this->_cmp_item=_int_cmp;
// }

// 初始化字符串类型向量
// static void _vector_init_string(vector this){
//     _init_size(this,sizeof(String));
//     this->_base=malloc(sizeof(String));
//     this->_copy_item=((string)this->_base)->copy;
//     this->_free_item=((string)this->_base)->free;
//     this->_cmp_item=((string)this->_base)->cmp;
//     this->_data_item=((string)this->_base)->data;
//     this->_in_data_item=((string)this->_base)->in_data;
// }

// 初始化书籍类型向量
// static void _vector_init_book(vector this){
//     _init_size(this,sizeof(Book));
//     this->_base=malloc(sizeof(Book));
//     this->_copy_item=((book)this->_base)->copy;
//     this->_free_item=((book)this->_base)->free;
//     this->_cmp_item=((book)this->_base)->cmp;
//     this->_data_item=((book)this->_base)->data;
//     this->_in_data_item=((book)this->_base)->in_data;
// }

// // 初始化学生类型向量
// static void _vector_init_student(vector this){
//     _init_size(this,sizeof(Student));
//     this->_base=malloc(sizeof(Student));
//     this->_copy_item=((student)this->_base)->copy;
//     this->_free_item=((student)this->_base)->free;
//     this->_cmp_item=((student)this->_base)->cmp;
//     this->_data_item=((student)this->_base)->data;
//     this->_in_data_item=((student)this->_base)->in_data;
// }

// 初始化向量大小
static void _init_size(vector this,size_t itemSize){
    this->npos=(size_t)-1;
    this->_itemSize=itemSize;
    this->_allocatedSize=10;
    this->_serialize=new_string();
    this->_typename=new_string();
    this->_data=malloc(this->_itemSize*this->_allocatedSize);
    assert(this->_data!=NULL);
    this->_size=0;
}

// 通用初始化类型函数
#define _init_define_type(this,TYPE) \
    ({ \
        _init_size(this, sizeof(TYPE)); \
        this->_base = malloc(sizeof(TYPE)); \
        this->_copy_item = ((TYPE *)this->_base)->copy; \
        this->_free_item = ((TYPE *)this->_base)->free; \
        this->_cmp_item = ((TYPE *)this->_base)->cmp; \
        this->_data_item = ((TYPE *)this->_base)->data; \
        this->_in_data_item = ((TYPE *)this->_base)->in_data; \
})

// 对所有支持的数据类型进行对应的初始化
static vector _init_type(vector this,const char *type){
    if(strcmp(type,"String")==0)
        _init_define_type(this,String);
    else if(strcmp(type,"Book")==0)
        _init_define_type(this,Book);
    else if(strcmp(type,"Student")==0)
        _init_define_type(this,Student);
    else{
        this->_copy_item=NULL;
        this->_free_item=NULL;
        this->_cmp_item=NULL;
        if(strcmp(type,"int")==0)
            _init_size(this,sizeof(int));
        else if(strcmp(type,"float")==0)
            _init_size(this,sizeof(float));
        else if(strcmp(type,"double")==0)
            _init_size(this,sizeof(double));
        else if(strcmp(type,"long long")==0)
            _init_size(this,sizeof(long long));
        else if(strcmp(type,"long double")==0)
            _init_size(this,sizeof(long double));
        else if(strcmp(type,"char")==0)
            _init_size(this,sizeof(char));
        else
            assert(0 && "Vector: _init_type 使用了不受支持的数据类型");
    }
    return this;
}

// 初始化向量函数指针
static vector _vector_init_func(vector this){
    this->push_back=_vector_push_back;
    this->erase=_vector_erase;
    this->at=_vector_at;
    this->size=_vector_size;
    this->find=_vector_find;
    this->free=_vector_free;
    this->data=_vector_data;
    this->in_data=_vector_in_data;
    this->resize=_vector_resize;
    this->init=_vector_init_func;
    // 类型初始化
    // this->iInt=_vector_init_int;
    // this->iString=_vector_init_string;
    // this->iBook=_vector_init_book;
    // this->iStudent=_vector_init_student;
    return this;
}

// 对已有的 Vector 初始化
void init_vector(vector this){
    _vector_init_func(this);
}

// 创建新的向量
vector new_vector(const char* type){
    vector this=malloc(sizeof(struct Vector));
    assert(this!=NULL);
    _init_type(this,type);
    return _init_func(this);
}
