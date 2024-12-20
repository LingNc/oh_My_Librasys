#include "Tools/String.h"
#include <stdlib.h>
#include <string.h>

// 前置声明所有函数
static char *_string_c_str(string this);
static string _string_assign_char(string this, char c);
static string _string_assign_cstr(string this, const char *s);
static string _string_assign(string this, string other);
static string _string_append_char(string this, char c);
static string _string_append_cstr(string this, const char *s);
static string _string_append_string(string this, string other);
static string _string_append_n(string this, const char *s, size_t n);
static string _string_insert_char(string this, size_t pos, char c);
static string _string_insert_cstr(string this, size_t pos, const char *s);
static string _string_insert_string(string this, size_t pos, string other);
static string _string_erase_char(string this, size_t pos);
static string _string_erase(string this, size_t pos, size_t len);
static size_t _string_find_char(string this, char c, size_t pos);
static size_t _string_find_cstr(string this, const char *s, size_t pos);
static size_t _string_find_string(string this, string other, size_t pos);
static int _string_cmp(string this, string other);
static size_t _string_size(string this);
static size_t _string_length_func(string this);
static char *_string_it(string this, size_t pos);
static char _string_at(string this, size_t pos);
static void _string_clear(string this);
static int _string_empty(string this);
static const char *_string_data(string this);
static void _string_in_data(string this, const char *data);
static void _string_free(string this);
static string _string_copy(string this, const string other);
static void _init_all(string this);
static size_t _string_rfind_char(string this, size_t pos, char c);
static size_t _string_rfind_cstr(string this, size_t pos, const char *s);
static size_t _string_rfind_string(string this, size_t pos, string other);
static void _string_push_back(string this, char c);
static void _string_pop_back(string this);
static void _grow(string this, size_t new_size);
static string _string_replace(string this, size_t pos, size_t len, const char *s);
static void _string_swap(string this, string other);
static string _string_init(string this);

// grow 函数实现
static void _grow(string this, size_t new_size) {
    if (new_size > this->_allocatedSize) {
        while (this->_allocatedSize < new_size) {
            this->_allocatedSize *= 2;
        }
        this->_data = (char *)realloc(this->_data, this->_allocatedSize);
    }
}

// c_str 函数实现
static char *_string_c_str(string this){
    return this->_data;
}

// assign_char 函数实现
static string _string_assign_char(string this, char c) {
    _grow(this, 2);
    this->_data[0] = c;
    this->_data[1] = '\0';
    this->_length = 1;
    return this;
}

// assign_cstr 函数实现
static string _string_assign_cstr(string this, const char *s) {
    size_t len = strlen(s);
    _grow(this, len + 1);
    memcpy(this->_data, s, len + 1);
    this->_length = len;
    return this;
}

// assign 函数实现
static string _string_assign(string this, string other) {
    return _string_assign_cstr(this, other->_data);
}

// append_char 函数实现
static string _string_append_char(string this,char c){
    _grow(this, this->_length + 2);
    this->_data[this->_length]=c;
    this->_data[this->_length+1]='\0';
    this->_length+=1;
    return this;
}

// append_cstr 函数实现
static string _string_append_cstr(string this,const char *s){
    size_t len=strlen(s);
    _grow(this, this->_length + len + 1);
    memcpy(this->_data+this->_length,s,len+1);
    this->_length+=len;
    return this;
}

// append_string 函数实现
static string _string_append_string(string this,string other){
    return _string_append_cstr(this,other->_data);
}

// append_n 函数实现
static string _string_append_n(string this, const char *s, size_t n) {
    // size_t len = strnlen(s, n);
    size_t len=n;
    _grow(this, this->_length + len + 1);
    memcpy(this->_data + this->_length, s, len);
    this->_length += len;
    this->_data[this->_length] = '\0';
    return this;
}

// insert_char 函数实现
static string _string_insert_char(string this,size_t pos,char c){
    if(pos>this->_length) pos=this->_length;
    _grow(this, this->_length + 2);
    memmove(this->_data+pos+1,this->_data+pos,this->_length-pos+1);
    this->_data[pos]=c;
    this->_length+=1;
    return this;
}

// insert_cstr 函数实现
static string _string_insert_cstr(string this,size_t pos,const char *s){
    size_t len=strlen(s);
    if(pos>this->_length) pos=this->_length;
    _grow(this, this->_length + len + 1);
    memmove(this->_data+pos+len,this->_data+pos,this->_length-pos+1);
    memcpy(this->_data+pos,s,len);
    this->_length+=len;
    return this;
}

// insert_string 函数实现
static string _string_insert_string(string this,size_t pos,string other){
    return _string_insert_cstr(this,pos,other->_data);
}

// erase_char 函数实现
static string _string_erase_char(string this,size_t pos){
    if(pos>=this->_length) return this;
    memmove(this->_data+pos,this->_data+pos+1,this->_length-pos);
    this->_length-=1;
    this->_data=(char *)realloc(this->_data,this->_length+1);
    return this;
}

// erase 函数实现
static string _string_erase(string this,size_t pos,size_t len){
    if(pos>=this->_length) return this;
    if(pos+len>this->_length) len=this->_length-pos;
    memmove(this->_data+pos,this->_data+pos+len,this->_length-pos-len+1);
    this->_length-=len;
    this->_data=(char *)realloc(this->_data,this->_length+1);
    return this;
}

// find_char 函数实现
static size_t _string_find_char(string this, char c, size_t pos){
    if(pos>=this->_length) return this->npos;
    char *p=strchr(this->_data+pos,c);
    if(p) return p-this->_data;
    else return this->npos;
}

// find_cstr 函数实现
static size_t _string_find_cstr(string this, const char *s, size_t pos){
    if(pos>=this->_length) return this->npos;
    char *p=strstr(this->_data+pos,s);
    if(p) return p-this->_data;
    else return this->npos;
}

// find_string 函数实现
static size_t _string_find_string(string this, string other, size_t pos){
    return _string_find_cstr(this, other->_data, pos);
}

// size 函数实现
static size_t _string_size(string this){
    return this->_length;
}

// length_func 函数实现
static size_t _string_length_func(string this){
    return this->_length;
}

// it 函数实现
static char *_string_it(string this,size_t pos){
    if(pos>=this->_length) return NULL;
    return this->_data+pos;
}

// at 函数实现
static char _string_at(string this,size_t pos){
    if(pos>=this->_length) return 0;
    return this->_data[pos];
}

// clear 函数实现
static void _string_clear(string this){
    free(this->_data);
    this->_data=(char *)malloc(1);
    this->_data[0]='\0';
    this->_length=0;
    this->_allocatedSize = 1;
}

// empty 函数实现
static int _string_empty(string this){
    return this->_length==0;
}

// rfind_char 函数实现
static size_t _string_rfind_char(string this, size_t pos, char c) {
    if (pos >= this->_length) pos = this->_length - 1;
    for (size_t i = pos; i != (size_t)(-1); --i) {
        if (this->_data[i] == c) return i;
    }
    return this->npos;
}

// rfind_cstr 函数实现
static size_t _string_rfind_cstr(string this, size_t pos, const char *s) {
    size_t len = strlen(s);
    if (len == 0) return this->npos;
    if (pos >= this->_length) pos = this->_length - 1;
    for (size_t i = pos + 1; i-- > 0;) {
        if (strncmp(this->_data + i, s, len) == 0) return i;
    }
    return this->npos;
}

// rfind_string 函数实现
static size_t _string_rfind_string(string this, size_t pos, string other) {
    return _string_rfind_cstr(this, pos, other->_data);
}

// push_back 函数实现
// 追加字符到字符串末尾
// 无返回值
static void _string_push_back(string this, char c) {
    _grow(this, this->_length + 2);
    this->_data[this->_length] = c;
    this->_data[this->_length + 1] = '\0';
    this->_length += 1;
}

// pop_back 函数实现
// 删除字符串末尾的字符
// 无返回值
static void _string_pop_back(string this) {
    if (this->_length == 0) return;
    this->_data[this->_length - 1] = '\0';
    this->_length -= 1;
    this->_data = (char *)realloc(this->_data, this->_length + 1);
}

// replace 函数实现
// 替换从pos位置开始长度为len的子字符串为s
// 返回替换后的字符串
static string _string_replace(string this, size_t pos, size_t len, const char *s) {
    if (pos > this->_length) pos = this->_length;
    if (pos + len > this->_length) len = this->_length - pos;
    size_t s_len = strlen(s);
    _grow(this, this->_length - len + s_len + 1);
    memmove(this->_data + pos + s_len, this->_data + pos + len, this->_length - pos - len + 1);
    memcpy(this->_data + pos, s, s_len);
    this->_length = this->_length - len + s_len;
    return this;
}

// swap 函数实现
// 交换两个字符串的内容
// 无返回值
static void _string_swap(string this, string other) {
    char *temp_data = this->_data;
    size_t temp_length = this->_length;
    size_t temp_allocatedSize = this->_allocatedSize;
    char *temp_serialize = this->_serialize;

    this->_data = other->_data;
    this->_length = other->_length;
    this->_allocatedSize = other->_allocatedSize;
    this->_serialize = other->_serialize;

    other->_data = temp_data;
    other->_length = temp_length;
    other->_allocatedSize = temp_allocatedSize;
    other->_serialize = temp_serialize;
}

// free 实现
static void _string_free(string this){
    free(this->_data);
    free(this->_serialize);
}

// copy 函数实现
static string _string_copy(string this,const string other){
    if(this==other) return this;
    _grow(this, other->_length + 1);
    memcpy(this->_data,other->_data,other->_length+1);
    this->_length=other->_length;
    return this;
}

// 字符串字典序对比
// 大于返回1，小于返回-1，等于0
static int _string_cmp(string this,string other){
    return strcmp(this->c_str(this), other->c_str(other));
}

// init 函数实现
static string _string_init(string this) {
    _init_all(this);
    return this;
}

// data 函数实现
static const char *_string_data(string this) {
    // 序列化数据包括字符串长度和字符串内容
    size_t out_size = sizeof(size_t) + this->_length;
    this->_serialize = (char *)realloc(this->_serialize, out_size);
    memcpy(this->_serialize, &this->_length, sizeof(size_t));
    memcpy(this->_serialize + sizeof(size_t), this->_data, this->_length);
    return this->_serialize;
}

// in_data 函数实现
static void _string_in_data(string this,const char *data){
    if(!data) return;
    size_t len;
    memcpy(&len,data,sizeof(size_t));
    _grow(this, len + 1);
    memcpy(this->_data,data+sizeof(size_t),len);
    this->_data[len]='\0';
    this->_length=len;
}

static void _init_all(string this){
    this->_data=(char *)malloc(1);
    this->_serialize=(char *)malloc(1);
    this->_data[0]='\0';
    this->_length=0;
    this->_allocatedSize = 1;
    this->npos=(size_t)(-1);

    // 赋值函数指针
    this->c_str=_string_c_str;
    this->assign_char=_string_assign_char;
    this->assign_cstr=_string_assign_cstr;
    this->assign=_string_assign;
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
    this->rfind_char = _string_rfind_char;
    this->rfind_cstr = _string_rfind_cstr;
    this->rfind = _string_rfind_string;
    this->push_back = _string_push_back;
    this->pop_back = _string_pop_back;
    this->replace = _string_replace;
    this->swap = _string_swap;
    this->init = _string_init;
    this->cmp = _string_cmp;
}

// 创建 String 对象的函数
string new_string(){
    string this=malloc(sizeof(String));
    _init_all(this);
    return this;
}

string __init_String(){
    return new_string();
}

void init_string(string this){
    _init_all(this);
}

// 销毁 String 对象的函数
void delete_string(string this){
    _string_free(this);
    free(this);
}