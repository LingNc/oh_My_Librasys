#include"Book.h"

// data获得序列化数据实现
const char *_book_data(book this){
    memset(this->serialize,0,sizeof(this->serialize));
    size_t len=sizeof(size_t)+sizeof(this->ISBN)+sizeof(this->status)+sizeof(this->author)+sizeof(this->publisher);
    size_t ptr=0;
    memcpy(this->serialize,&len,sizeof(len));
    ptr+=sizeof(len);

    memcpy(this->serialize+ptr,this->ISBN,sizeof(this->ISBN));
    ptr+=sizeof(this->ISBN);

    memcpy(this->serialize+ptr,&this->status,sizeof(this->status));
    ptr+=sizeof(this->status);

    memcpy(this->serialize+ptr,this->author,sizeof(this->author));
    ptr+=sizeof(this->author);

    memcpy(this->serialize+ptr,this->publisher,sizeof(this->publisher));
    ptr+=sizeof(this->publisher);

    return this->serialize;
}

int _book_in_data(book this,const char *data){
    size_t ptr=0;
    size_t len=0;
    memcpy(&len,data+ptr,sizeof(len));
    ptr+=sizeof(len);

    memcpy(this->ISBN,data+ptr,sizeof(this->ISBN));
    ptr+=sizeof(this->ISBN);

    memcpy(&this->status,data+ptr,sizeof(this->status));
    ptr+=sizeof(this->status);

    memcpy(this->author,data+ptr,sizeof(this->author));
    ptr+=sizeof(this->author);

    memcpy(this->publisher,data+ptr,sizeof(this->publisher));
    ptr+=sizeof(this->publisher);

    return 1; // 表示成功
}