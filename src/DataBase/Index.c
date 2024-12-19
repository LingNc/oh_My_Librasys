#include "DataBase/Index.h"
#include "Tools/Pair.h"
#include <stdio.h>
#include <stdlib.h>

// 简单的哈希函数
static size_t default_hash_func(size_t key) {
    return key;
}

// 初始化索引
void init_index(database_index index, const char *filePath, size_t bucket_count) {
    index->filePath = new_string();
    index->filePath->assign_cstr(index->filePath, filePath);
    // index->filePath->append_cstr(index->filePath,".idx");
    index->hash = default_hash_func;
    index->bucket_count = bucket_count;
    index->nums = 0; // 初始化索引数量
    index->buckets = (vector *)malloc(bucket_count * sizeof(vector));
    if (!index->buckets) {
        perror("Index: buckets 指针分配失败");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < bucket_count; ++i) {
        index->buckets[i] = vector(Pair);
    }
}

// 关闭索引
void close_index(database_index index) {
    for (size_t i = 0; i < index->bucket_count; ++i) {
        vector bucket = index->buckets[i];
        for (size_t j = 0; j < bucket->size(bucket); ++j) {
            pair p = (pair)bucket->at(bucket, j);
            free_pair(p);
        }
        bucket->free(bucket);
    }
    free(index->buckets);
    delete_string(index->filePath);
    free(index);
}

// 通过键查找对应的偏移量
size_t find_index(database_index index, size_t key) {
    size_t hash = index->hash(key);
    size_t bucket_index = hash % index->bucket_count;
    vector bucket = index->buckets[bucket_index];
    for (size_t i = 0; i < bucket->size(bucket); ++i) {
        pair p = (pair)bucket->at(bucket, i);
        if (p->key == key) {
            return p->value;
        }
    }
    return 0; // 未找到返回0
}

// 添加键值对到索引
void add_index(database_index index, size_t key, size_t offset) {
    size_t hash = index->hash(key);
    size_t bucket_index = hash % index->bucket_count;
    vector bucket = index->buckets[bucket_index];
    pair p = new_pair();
    init_pairs(p, key, offset);
    bucket->push_back(bucket, p);
    index->nums++; // 增加索引数量
}

// 删除键值对
void remove_index(database_index index, size_t key) {
    size_t hash = index->hash(key);
    size_t bucket_index = hash % index->bucket_count;
    vector bucket = index->buckets[bucket_index];
    for (size_t i = 0; i < bucket->size(bucket); ++i) {
        pair p = (pair)bucket->at(bucket, i);
        if (p->key == key) {
            bucket->remove(bucket, i);
            free_pair(p);
            index->nums--; // 减少索引数量
            return;
        }
    }
}

// 获取最后一个索引键
size_t get_last_index_key(database_index index) {
    size_t lastKey = 0;
    for (size_t i = 0; i < index->bucket_count; ++i) {
        vector bucket = index->buckets[i];
        for (size_t j = 0; j < bucket->size(bucket); ++j) {
            pair p = (pair)bucket->at(bucket, j);
            if (p->key > lastKey) {
                lastKey = p->key;
            }
        }
    }
    return lastKey;
}

// 从文件加载索引
void load_index(database_index index) {
    char indexFilePath[256];
    snprintf(indexFilePath, sizeof(indexFilePath), "%s.idx", index->filePath->c_str(index->filePath));
    FILE *indexFile = fopen(indexFilePath, "rb");
    if (!indexFile) {
        perror("index: 无法打开索引文件进行读取，正在初始化新索引\n");
        init_index(index, index->filePath->c_str(index->filePath), index->bucket_count);
        return;
    }
    size_t indexCount;
    fread(&indexCount, sizeof(size_t), 1, indexFile);
    // index->nums = indexCount; // 设置索引数量
    for (size_t i = 0; i < indexCount; ++i) {
        size_t key, offset;
        fread(&key, sizeof(size_t), 1, indexFile);
        fread(&offset, sizeof(size_t), 1, indexFile);
        add_index(index, key, offset);
    }
    fclose(indexFile);
}

// 保存索引到文件
void save_index(database_index index) {
    char indexFilePath[256];
    snprintf(indexFilePath, sizeof(indexFilePath), "%s.idx", index->filePath->c_str(index->filePath));
    FILE *indexFile = fopen(indexFilePath, "wb");
    if (!indexFile) {
        perror("index: 不能打开index文件,可能文件夹不存在\n");
        return;
    }
    fwrite(&index->nums, sizeof(size_t), 1, indexFile); // 写入索引数量
    for (size_t i = 0; i < index->bucket_count; ++i) {
        vector bucket = index->buckets[i];
        for (size_t j = 0; j < bucket->size(bucket); ++j) {
            pair p = (pair)bucket->at(bucket, j);
            size_t key = p->key;
            size_t offset = p->value;
            fwrite(&key, sizeof(size_t), 1, indexFile);
            fwrite(&offset, sizeof(size_t), 1, indexFile);
        }
    }
    fclose(indexFile);
}

// 重建索引
void rebuild_index(database_index index, const char *filePath) {
    clear_index(index);
    FILE *file = fopen(filePath, "rb");
    if (!file) {
        perror("index: 无法打开文件进行读取");
        return;
    }
    fseek(file, sizeof(size_t), SEEK_SET); // 跳过数据数量
    size_t offset = sizeof(size_t);
    size_t dataSize;
    while (fread(&dataSize, sizeof(size_t), 1, file) == 1) {
        char isDeleted;
        fread(&isDeleted, sizeof(char), 1, file);
        if (isDeleted == 1) {
            size_t key;
            fread(&key, sizeof(size_t), 1, file);
            add_index(index, key, offset);
            fseek(file, dataSize - sizeof(size_t), SEEK_CUR); // 跳过剩余数据
        } else {
            fseek(file, dataSize, SEEK_CUR);
        }
        offset += sizeof(size_t) + sizeof(char) + dataSize;
    }
    fclose(file);
}

// 清空索引
void clear_index(database_index index) {
    for (size_t i = 0; i < index->bucket_count; ++i) {
        vector bucket = index->buckets[i];
        for (size_t j = 0; j < bucket->size(bucket); ++j) {
            pair p = (pair)bucket->at(bucket, j);
            free_pair(p);
        }
        bucket->clear(bucket);
    }
}

