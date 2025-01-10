#ifndef _manager_lib
#define _manager_lib

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Tools/String.h"
#include "Tools/Vector.h"

typedef struct Manager Manager;
typedef Manager *manager;

struct Manager {
    size_t id;
    string name;
    string registration_date;
    string registered_by;

    // 序列化数据
    string _serialize;
    _init_default_func(manager);
};

void load_manager(manager this, size_t id, const char *name, const char *registration_date, const char *registered_by);

// 初始化 Manager 对象
manager __init_Manager();
manager new_manager();

// 释放 Manager 对象本身
void free_manager(manager this);

#endif