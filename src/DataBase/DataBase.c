#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
static void _init_all(dataBase this,const char *filePath);
static void _database_add(dataBase this,void *data);
static void _database_remove(dataBase this,size_t key);
static void _database_save(dataBase this);
static vector _database_find(dataBase this,const void *data);
static void *_database_find_key(dataBase this,size_t key);
static void _init_func(dataBase this);
static size_t _database_size(dataBase this);
static vector _database_get(dataBase this,size_t key,size_t nums);
static vector _database_get_find(dataBase this,size_t pos,size_t nums);
static void _database_add_auto(dataBase this,void* data);
static void _database_add_key(dataBase this, void* data, size_t key);
void clean_database(dataBase this);

#define _max_item 100

// 获得数据库的实际储存大小
static size_t _database_size(dataBase this){
    return this->_index->nums;
}

// 加载数据库
dataBase load_database(const char *inPath,const char *type){
    dataBase this=(dataBase)malloc(sizeof(DataBase));
    _init_func(this);
    this->_type=new_string();
    this->_type->assign_cstr(this->_type,type);
    _init_all(this,inPath);

    // 检查数据库文件
    FILE *file=fopen(this->filePath->c_str(this->filePath),"a+b");
    // 检查文件是否为空，如果为空则初始化文件内容
    fseek(file,0,SEEK_END);
    long fileSize=ftell(file);
    if(fileSize==0){
        // 文件为空，初始化文件内容
        size_t dataCount=0;
        fwrite(&dataCount,sizeof(size_t),1,file);
        fflush(file);
    }
    fclose(file);
    return this;
}

// 初始化函数
static void _init_func(dataBase this){
    this->add=_database_add;
    this->rm=_database_remove;
    this->save=_database_save;
    this->find=_database_find;
    this->find_key=_database_find_key;
    this->clean=clean_database;
    this->get=_database_get;
    this->get_find=_database_get_find;
    this->add_key = _database_add_key;
    this->add_auto = _database_add_auto;
}

// 初始化数据库
static void _init_all(dataBase this,const char *inPath){
    this->_inPath=new_string();
    this->_inPath->assign_cstr(this->_inPath,inPath);

    this->filePath=new_string();
    this->filePath->assign_cstr(this->filePath,inPath);
    this->filePath->append_cstr(this->filePath,".db");

    this->_buffer=new_vector(this->_type->c_str(this->_type));
    this->_temp=new_vector(this->_type->c_str(this->_type));
    this->_find_buffer=new_vector(this->_type->c_str(this->_type));
    this->_index=(database_index)malloc(sizeof(DataBase_Index));
    init_index(this->_index,inPath,100);
    load_index(this->_index);
}

// 添加数据,自动索引
static void _database_add(dataBase this,void *data){
    size_t newKey=get_next_index_key(this->_index); // 获取索引中最后一个唯一编号id，然后向后顺延一个
    // id在第1个
    *(size_t *)data=newKey;
    this->_buffer->push_back(this->_buffer,data);
}
// 添加数据,根据读入数据自动索引
static void _database_add_auto(dataBase this,void* data){
    this->_buffer->push_back(this->_buffer,data);
}
// 添加数据,手动索引
static void _database_add_key(dataBase this,void* data,size_t key){
    *(size_t*)data=key;
    this->_buffer->push_back(this->_buffer,data);
}

// 删除数据
static void _database_remove(dataBase this,size_t key){
    size_t offset=find_index(this->_index,key);
    if(offset!=0){
        FILE *file=fopen(this->filePath->c_str(this->filePath),"rb+");
        if(!file){
            perror("DataBase: 无法打开文件进行写入");
            return;
        }
        fseek(file,offset,SEEK_SET);
        char isDeleted=0;
        fwrite(&isDeleted,sizeof(char),1,file);
        fclose(file);
        remove_index(this->_index,key);
    }
}

// 保存数据库到文件
static void _database_save(dataBase this) {
    FILE *file = fopen(this->filePath->c_str(this->filePath), "ab");
    if (!file) {
        perror("DataBase: 不能打开文件,可能文件夹不存在\n");
        return;
    }
    size_t dataCount = this->_buffer->size(this->_buffer);
    // fwrite(&dataCount, sizeof(size_t), 1, file);
    for (size_t i = 0; i < dataCount; ++i) {
        void *data = this->_buffer->at(this->_buffer, i);
        const char *serializedData = this->_buffer->_data_item(data);
        size_t dataSize = *(size_t*)serializedData+sizeof(size_t);
        char isDeleted = 1;
        size_t offset = ftell(file);
        fwrite(&isDeleted, sizeof(char), 1, file);
        fwrite(serializedData, dataSize, 1, file);
        size_t key = *(size_t*)data;
        add_index(this->_index, key, offset);
    }
    fclose(file);
    this->_buffer->clear(this->_buffer);

    // 保存索引到单独的文件
    save_index(this->_index);
}

// 查找数据
static vector _database_find(dataBase this,const void *data){
    this->_find_buffer->clear(this->_find_buffer);
    FILE *file=fopen(this->filePath->c_str(this->filePath),"rb");
    if(!file){
        perror("DataBase: 无法打开文件进行读取");
        return this->_find_buffer;
    }
    fseek(file,sizeof(size_t),SEEK_SET); // 跳过数据数量
    char isDeleted;
    while(fread(&isDeleted,sizeof(char),1,file)==1){
        size_t dataSize;
        fread(&dataSize,sizeof(size_t),1,file);
        // 设置索引的位置由于第一个sizet是后面数据块的大小而数据初始化需要整个块，进行文件指针的移动
        fseek(file,-sizeof(size_t),SEEK_CUR);
        dataSize+=sizeof(size_t);

        if(isDeleted==1){
            char *buffer=(char *)malloc(dataSize);
            fread(buffer,dataSize,1,file);
            void *item=malloc(this->_find_buffer->_itemSize);
            this->_find_buffer->_init_item(item);
            this->_find_buffer->_in_data_item(item,buffer);
            if(this->_find_buffer->_cmp_item(item,data)>=0){
                this->_find_buffer->push_back(this->_find_buffer,item);
            }
            else{
                free(item);
            }
            free(buffer);
        }
        else{
            fseek(file,dataSize,SEEK_CUR);
        }
    }
    fclose(file);
    return this->_find_buffer;
}

// 通过键查找数据
static void *_database_find_key(dataBase this, size_t key) {
    size_t offset = find_index(this->_index, key);
    if (offset != 0) {
        FILE *file = fopen(this->filePath->c_str(this->filePath), "rb");
        if (!file) {
            perror("DataBase: 无法打开文件进行读取");
            return NULL;
        }
        fseek(file, offset, SEEK_SET);
        char isDeleted;
        fread(&isDeleted, sizeof(char), 1, file);
        size_t dataSize;
        fread(&dataSize, sizeof(size_t), 1, file);
        fseek(file,-sizeof(size_t),SEEK_CUR);
        if (isDeleted == 1) {
            char *data = (char *)malloc(dataSize);
            fread(data, dataSize, 1, file);
            void *item = malloc(this->_find_buffer->_itemSize);
            this->_find_buffer->_init_item(item);
            this->_find_buffer->_in_data_item(item, data);
            free(data);
            fclose(file);
            return item;
        }
        fclose(file);
    }
    return NULL;
}

// 从数据库根据key 得到索引，从第pos开始，nums个东西
static vector _database_get(dataBase this,size_t key,size_t nums){
    vector result=new_vector(this->_type->c_str(this->_type));
    size_t count=0;
    for(size_t i=key; i<this->_index->nums&&count<nums; ++i){
        // size_t key=get_index_key(this->_index,i);
        void *data=_database_find_key(this,i);
        if(data){
            result->push_back(result,data);
            ++count;
        }
    }
    return result;
}

// 从_find_buffer 得到索引，从第pos开始，nums个东西
static vector _database_get_find(dataBase this,size_t pos,size_t nums){
    vector result=new_vector(this->_type->c_str(this->_type));
    size_t count=0;
    for(size_t i=pos; i<this->_find_buffer->size(this->_find_buffer)&&count<nums; ++i){
        void *data=this->_find_buffer->at(this->_find_buffer,i);
        result->push_back(result,data);
        ++count;
    }
    return result;
}

// 关闭数据库
void close_database(dataBase this){
    _database_save(this);
    this->_buffer->clear(this->_buffer);
    this->_temp->clear(this->_temp);
    this->_find_buffer->clear(this->_find_buffer);
    close_index(this->_index);
    delete_string(this->filePath);
    delete_string(this->_type);
    free(this);
}

// 清理数据库
void clean_database(dataBase this){
    FILE *file=fopen(this->filePath->c_str(this->filePath),"rb+");
    if(!file){
        perror("DataBase: 无法打开文件进行清理");
        return;
    }
    FILE *tempFile=fopen("temp.db","wb");
    if(!tempFile){
        perror("DataBase: 无法创建临时文件");
        fclose(file);
        return;
    }
    fseek(file,sizeof(size_t),SEEK_SET); // 跳过数据数量
    size_t offset=sizeof(size_t);
    size_t dataSize;
    while(fread(&dataSize,sizeof(size_t),1,file)==1){
        char isDeleted;
        fread(&isDeleted,sizeof(char),1,file);
        if(isDeleted==1){
            size_t key;
            fread(&key,sizeof(size_t),1,file);
            fwrite(&dataSize,sizeof(size_t),1,tempFile);
            fwrite(&isDeleted,sizeof(char),1,tempFile);
            fwrite(&key,sizeof(size_t),1,tempFile);
            char *data=(char *)malloc(dataSize-sizeof(size_t));
            fread(data,dataSize-sizeof(size_t),1,file);
            fwrite(data,dataSize-sizeof(size_t),1,tempFile);
            free(data);
        }
        else{
            fseek(file,dataSize,SEEK_CUR);
        }
        offset+=sizeof(size_t)+sizeof(char)+dataSize;
    }
    fclose(file);
    fclose(tempFile);
    remove(this->filePath->c_str(this->filePath));
    rename("temp.db",this->filePath->c_str(this->filePath));
    rebuild_index(this->_index,this->filePath->c_str(this->filePath));
}

// 获取下一个索引键
size_t get_next_index_key(database_index index){
    size_t lastKey=get_last_index_key(index); // 获取索引中最后一个唯一编号id
    return lastKey+1;
}


