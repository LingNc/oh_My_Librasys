#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
static void _init_all(dataBase this,const char *filePath);
static void _database_add(dataBase this,void *data);
// static void _database_add_no_key(dataBase this, void* data);
static void _database_remove(dataBase this,size_t key);
static void _database_save(dataBase this);
static size_t _database_find(dataBase this, size_t nums, const void *data);
static void *_database_find_key(dataBase this,size_t key);
static void _init_func(dataBase this);
static size_t _database_size(dataBase this);
static vector _database_get(dataBase this,size_t key,size_t nums);
static vector _database_get_find(dataBase this,size_t pos,size_t nums, const void *data);
static void _database_add_auto(dataBase this,void *data);
static void _database_add_key(dataBase this,void *data,size_t key);
static bool _database_change(dataBase this,size_t id,void *new_data);
void clean_database(dataBase this);

#define _max_item 100

// 获取数据库的实际储存大小
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
    this->add_key=_database_add_key;
    this->add_auto=_database_add_auto;
    this->size=_database_size;
    this->change=_database_change;
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
    init_index(this->_index,inPath,BUCKET_NUMS);
    load_index(this->_index);
    this->_buffer_index=new_vector("size_t"); // 初始化索引缓冲区
}

// 添加数据,自动索引
static void _database_add(dataBase this,void *data){
    size_t newKey = get_new_key(this->_index); // 获取第一个未使用的唯一编号id
    *(size_t *)data = newKey;
    this->_buffer->push_back(this->_buffer, data);
    this->_buffer_index->push_back(this->_buffer_index, &newKey); // 将新键值对存入索引缓冲区
    // this->_index->_last_key = newKey; // 更新_last_key
}

// 添加数据,根据读入数据自动索引
static void _database_add_auto(dataBase this,void *data){
    size_t newKey = *(size_t *)data; // 从数据中读取id
    this->_buffer->push_back(this->_buffer, data);
    this->_buffer_index->push_back(this->_buffer_index, &newKey); // 将新键值对存入索引缓冲区
    // if (newKey > this->_index->_last_key) {
    //     this->_index->_last_key = newKey; // 更新_last_key
    // }
}

// 添加数据,手动索引
static void _database_add_key(dataBase this,void *data,size_t key){
    this->_buffer->push_back(this->_buffer,data);
    this->_buffer_index->push_back(this->_buffer_index,&key); // 将新键值对存入索引缓冲区
}

// // 添加数据,不使用索引
// static void _database_add_no_key(dataBase this, void* data) {
//     this->_buffer->push_back(this->_buffer, data);
// }

// 获取数据
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
static void _database_save(dataBase this){
    FILE *file=fopen(this->filePath->c_str(this->filePath),"ab");
    if(!file){
        perror("DataBase: 不能打开文件,可能文件夹不存在\n");
        return;
    }
    size_t dataCount=this->_buffer->size(this->_buffer);
    for(size_t i=0; i<dataCount; ++i){
        void *data=this->_buffer->at(this->_buffer,i);
        size_t *key=(size_t *)this->_buffer_index->at(this->_buffer_index,i); // 从索引缓冲区获取键值对
        const char *serializedData=this->_buffer->_data_item(data);
        size_t dataSize=*(size_t *)serializedData+sizeof(size_t);
        char isDeleted=1;
        size_t offset=ftell(file);
        fwrite(&isDeleted,sizeof(char),1,file);
        fwrite(serializedData,dataSize,1,file);
        add_index(this->_index,*key,offset); // 使用键值对添加索引
    }
    fclose(file);
    this->_buffer->clear(this->_buffer);
    this->_buffer_index->clear(this->_buffer_index); // 清空索引缓冲��

    // 保存索引到单独的文件
    save_index(this->_index);
}

// 查找数据
static size_t _database_find(dataBase this, size_t nums, const void *data) {
    this->_find_buffer->clear(this->_find_buffer);
    FILE *file = fopen(this->filePath->c_str(this->filePath), "rb");
    if (!file) {
        perror("DataBase: 无法打开文件进行读取");
        return 0;
    }
    fseek(file, sizeof(size_t), SEEK_SET); // 跳过数据数量
    char isDeleted;
    size_t found_count = 0;
    size_t last_key = 0;
    while (fread(&isDeleted, sizeof(char), 1, file) == 1) {
        size_t dataSize;
        fread(&dataSize, sizeof(size_t), 1, file);
        fseek(file, -sizeof(size_t), SEEK_CUR);
        dataSize += sizeof(size_t);

        if (isDeleted == 1) {
            char *buffer = (char *)malloc(dataSize);
            fread(buffer, dataSize, 1, file);
            vector buff = this->_find_buffer;
            void *item = malloc(buff->_itemSize);
            if (this->_find_buffer->_init_item) this->_find_buffer->_init_item(item);
            this->_find_buffer->_in_data_item(item, buffer);
            if (this->_find_buffer->_cmp_item) {
                if (this->_find_buffer->_cmp_item(item, data) >= 0) {
                    buff->push_back(buff, item);
                    found_count++;
                } else {
                    free(item);
                }
            } else {
                if (buff->_dcmp_item(item, data, buff->_typename->c_str(buff->_typename)) == 0) {
                    buff->push_back(buff, item);
                    found_count++;
                } else {
                    free(item);
                }
            }
            free(buffer);
            if (found_count >= nums) {
                last_key = *(size_t *)item;
                break;
            }
        } else {
            fseek(file, dataSize, SEEK_CUR);
        }
    }
    fclose(file);
    return last_key + 1;
}

// 通过键查找数据
static void *_database_find_key(dataBase this,size_t key){
    size_t offset=find_index(this->_index,key);
    if(offset!=0){
        FILE *file=fopen(this->filePath->c_str(this->filePath),"rb");
        if(!file){
            perror("DataBase: 无法打开文件进行读取");
            return NULL;
        }
        fseek(file,offset,SEEK_SET);
        char isDeleted;
        fread(&isDeleted,sizeof(char),1,file);
        size_t dataSize;
        fread(&dataSize,sizeof(size_t),1,file);
        fseek(file,-sizeof(size_t),SEEK_CUR);
        dataSize+=sizeof(size_t);
        if(isDeleted==1){
            char *data=(char *)malloc(dataSize);
            fread(data,dataSize,1,file);
            void *item=malloc(this->_find_buffer->_itemSize);
            this->_find_buffer->_init_item(item);
            this->_find_buffer->_in_data_item(item,data);
            free(data);
            fclose(file);
            return item;
        }
        fclose(file);
    }
    return NULL;
}

// 从数据库根据key 得到索引，从pos开始，nums个东西
static vector _database_get(dataBase this,size_t key,size_t nums){
    vector result=new_vector(this->_type->c_str(this->_type));
    size_t count=0;
    for(size_t i=key; (i<=this->_index->nums)&&(count<nums); ++i){
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
static vector _database_get_find(dataBase this, size_t pos, size_t nums, const void *data) {
    vector result = new_vector(this->_type->c_str(this->_type));
    size_t count = 0;
    size_t buffer_size = this->_find_buffer->size(this->_find_buffer);

    // 从_find_buffer 获取数据
    for (size_t i = pos; i < buffer_size && count < nums; ++i) {
        void *item = this->_find_buffer->at(this->_find_buffer, i);
        result->push_back(result, item);
        ++count;
    }

    // 如果不足nums个，继续查找
    while (count < nums) {
        size_t next_key = this->find(this, nums - count, data);
        if (next_key == 0) {
            break; // 已经遍历完所有数据
        }
        buffer_size = this->_find_buffer->size(this->_find_buffer);
        for (size_t i = 0; i < buffer_size && count < nums; ++i) {
            void *item = this->_find_buffer->at(this->_find_buffer, i);
            result->push_back(result, item);
            ++count;
        }
    }

    return result;
}

// 关闭数据库
void close_database(dataBase this){
    _database_save(this);
    delete_vector(this->_buffer);
    delete_vector(this->_buffer_index);
    delete_vector(this->_temp);
    delete_vector(this->_find_buffer);
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

// 获取取下标个索引键
size_t get_new_key(database_index index);

// 修改数据
static bool _database_change(dataBase this,size_t id,void *new_data){
    // 先删除旧数据
    _database_remove(this,id);
    // 再添加新数据
    _database_add_key(this,new_data,id);
    // 保存
    _database_save(this);
    return true;
}


