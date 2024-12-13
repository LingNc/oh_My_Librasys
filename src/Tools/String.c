#include "Tools/String.h"
#include <stdlib.h>
#include <string.h>

// c_str 函数实现
char *_string_c_str(string this){
    return this->_data;
}

// write_char 函数实现
string _string_write_char(string this,char c){
    this->_data=(char *)realloc(this->_data,2);
    this->_data[0]=c;
    this->_data[1]='\0';
    this->_length=1;
    return this;
}

// write_cstr 函数实现
string _string_write_cstr(string this,const char *s){
    size_t len=strlen(s);
    this->_data=(char *)realloc(this->_data,len+1);
    memcpy(this->_data,s,len+1);
    this->_length=len;
    return this;
}

// write_string 函数实现
string _string_write_string(string this,string other){
    return _string_write_cstr(this,other->_data);
}

// append_char 函数实现
string _string_append_char(string this,char c){
    this->_data=(char *)realloc(this->_data,this->_length+2);
    this->_data[this->_length]=c;
    this->_data[this->_length+1]='\0';
    this->_length+=1;
    return this;
}

// append_cstr 函数��现
string _string_append_cstr(string this,const char *s){
    size_t len=strlen(s);
    this->_data=(char *)realloc(this->_data,this->_length+len+1);
    memcpy(this->_data+this->_length,s,len+1);
    this->_length+=len;
    return this;
}

// append_string 函数实现
string _string_append_string(string this,string other){
    return _string_append_cstr(this,other->_data);
}

// append_n 函数实现
string _string_append_n(string this, const char *s, size_t n) {
    size_t len = strnlen(s, n);
    this->_data = (char *)realloc(this->_data, this->_length + len + 1);
    memcpy(this->_data + this->_length, s, len);
    this->_length += len;
    this->_data[this->_length] = '\0';
    return this;
}

// insert_char 函数实现
string _string_insert_char(string this,size_t pos,char c){
    if(pos>this->_length) pos=this->_length;
    this->_data=(char *)realloc(this->_data,this->_length+2);
    memmove(this->_data+pos+1,this->_data+pos,this->_length-pos+1);
    this->_data[pos]=c;
    this->_length+=1;
    return this;
}

// insert_cstr 函数实现
string _string_insert_cstr(string this,size_t pos,const char *s){
    size_t len=strlen(s);
    if(pos>this->_length) pos=this->_length;
    this->_data=(char *)realloc(this->_data,this->_length+len+1);
    memmove(this->_data+pos+len,this->_data+pos,this->_length-pos+1);
    memcpy(this->_data+pos,s,len);
    this->_length+=len;
    return this;
}

// insert_string 函数实现
string _string_insert_string(string this,size_t pos,string other){
    return _string_insert_cstr(this,pos,other->_data);
}

// erase_char 函数实现
string _string_erase_char(string this,size_t pos){
    if(pos>=this->_length) return this;
    memmove(this->_data+pos,this->_data+pos+1,this->_length-pos);
    this->_length-=1;
    this->_data=(char *)realloc(this->_data,this->_length+1);
    return this;
}

// erase 函数实现
string _string_erase(string this,size_t pos,size_t len){
    if(pos>=this->_length) return this;
    if(pos+len>this->_length) len=this->_length-pos;
    memmove(this->_data+pos,this->_data+pos+len,this->_length-pos-len+1);
    this->_length-=len;
    this->_data=(char *)realloc(this->_data,this->_length+1);
    return this;
}

// find_char 函数实现
size_t _string_find_char(string this,size_t pos,char c){
    if(pos>=this->_length) return this->npos;
    char *p=strchr(this->_data+pos,c);
    if(p) return p-this->_data;
    else return this->npos;
}

// find_cstr 函数实现
size_t _string_find_cstr(string this,size_t pos,const char *s){
    if(pos>=this->_length) return this->npos;
    char *p=strstr(this->_data+pos,s);
    if(p) return p-this->_data;
    else return this->npos;
}

// find_string 函数实现
size_t _string_find_string(string this,size_t pos,string other){
    return _string_find_cstr(this,pos,other->_data);
}

// 字符串字典序对比
// 大于返回1，小于返回-1，等于0
int _string_cmp(string this,string other){
    return strcmp(((string)this)->c_str(this),((string)other)->c_str(other));
}

// size 函数实现
size_t _string_size(string this){
    return this->_length;
}

// length_func 函数实现
size_t _string_length_func(string this){
    return this->_length;
}

// it 函数实现
char _string_it(string this,size_t pos){
    if(pos>=this->_length) return '\0';
    return this->_data[pos];
}

// at 函数实现
char *_string_at(string this,size_t pos){
    if(pos>=this->_length) return NULL;
    return this->_data+pos;
}

// clear 函数实现
void _string_clear(string this){
    free(this->_data);
    this->_data=(char *)malloc(1);
    this->_data[0]='\0';
    this->_length=0;
}

// empty 函数实现
int _string_empty(string this){
    return this->_length==0;
}

// data 函数实现
const char *_string_data(string this){
    // 序列化数据包括字符串长度和字符串内容
    size_t out_size=sizeof(size_t)+this->_length;
    this->_serialize=(char *)realloc(this->_serialize,
        out_size);
    memcpy(this->_serialize,&this->_length,sizeof(size_t));
    memcpy(this->_serialize+sizeof(size_t),this->_data,this->_length);
    return this->_serialize;
}

// in_data 函数实现
bool _string_in_data(string this,const char *data){
    if(!data) return false;
    size_t len;
    memcpy(&len,data,sizeof(size_t));
    this->_data=(char *)realloc(this->_data,len+1);
    if(!this->_data) return false;
    memcpy(this->_data,data+sizeof(size_t),len);
    this->_data[len]='\0';
    this->_length=len;
    return true;
}
// free 实现
void _string_free(string this){
    delete_string(this);
}

// copy 函数实现
string _string_copy(string this,string other){
    if(this==other) return this;
    this->_data=(char *)realloc(this->_data,other->_length+1);
    memcpy(this->_data,other->_data,other->_length+1);
    this->_length=other->_length;
    return this;
}
void _init(string this){
    this->_data=(char *)malloc(1);
    this->_serialize=(char *)malloc(1);
    this->_data[0]='\0';
    this->_length=0;
    this->npos=(size_t)(-1);

    // 赋值函数指针
    this->c_str=_string_c_str;
    this->write_char=_string_write_char;
    this->write_cstr=_string_write_cstr;
    this->write=_string_write_string;
    this->append_char=_string_append_char;
    this->append_cstr=_string_append_cstr;
    this->append=_string_append_string;
    this->append_n=_string_append_n;
    this->insert_char=_string_insert_char;
    this->insert_cstr=_string_insert_cstr;
    this->insert=_string_insert_string;
    this->erase_char=_string_erase_char;
    this->erase=_string_erase;
    this->find_char=_string_find_char;
    this->find_cstr=_string_find_cstr;
    this->find=_string_find_string;
    this->size=_string_size;
    this->length=_string_length_func;
    this->at=_string_at;
    this->it=_string_it;
    this->clear=_string_clear;
    this->empty=_string_empty;
    this->data=_string_data;
    this->in_data=_string_in_data;
    this->free=_string_free;
    this->copy=_string_copy;
}
// 创建 String 对象的函数
string new_string(){
    string this=(string)malloc(sizeof(string));
    _init(this);
    return this;
}

void init_string(string this){
    _init(this);
}
// 销毁 String 对象的函数
void delete_string(string this){
    free(this->_data);
    free(this->_serialize);
}