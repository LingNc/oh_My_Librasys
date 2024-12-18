#include "Tools/Type.h"
#include "Student.h"
#include "Tools/String.h"
#include "Book.h"
#include "Tools/Vector.h"
#include "Tools/List.h"
#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 默认比较函数，非自定义结构体的比较函数
int _default_cmp(const void *a,const void *b,size_t itemSize){
    _default_cmp_if_for(int)
    else _default_cmp_if_for(float)
    else _default_cmp_if_for(double)
    else _default_cmp_if_for(long long)
    else _default_cmp_if_for(long double)
    else _default_cmp_if_for(char)
    else{
        assert(0&&"_default_cmp 传入了不支持的比较类型");
        return 0;
    }
}

// void *new_item(const char *type) {
//     void *item = NULL;
//     if (strcmp(type, "int") == 0) {
//         item = malloc(sizeof(int));
//     } else if (strcmp(type, "float") == 0) {
//         item = malloc(sizeof(float));
//     } else if (strcmp(type, "double") == 0) {
//         item = malloc(sizeof(double));
//     } else if (strcmp(type, "long long") == 0) {
//         item = malloc(sizeof(long long));
//     } else if (strcmp(type, "long double") == 0) {
//         item = malloc(sizeof(long double));
//     } else if (strcmp(type, "char") == 0) {
//         item = malloc(sizeof(char));
//     } else {
//         _init_type_for_custom(Student)
//         else _init_type_for_custom(String)
//         else _init_type_for_custom(Book)
//         else {
//             assert(0 && "new_item 使用了不受支持的数据类型");
//         }
//     }
//     return item;
// }