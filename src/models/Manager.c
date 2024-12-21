#include "models/Manager.h"

// 函数声明
manager _manager_init(manager this);
manager new_manager();
manager __init_Manager();
void load_manager(manager this,size_t id,const char *name,const char *registration_date,const char *registered_by);
void _manager_free(manager this);
void free_manager(manager this);
manager _manager_copy(manager this, manager other);
int _manager_cmp(manager this, manager other);
const char *_manager_data(manager this);
void _manager_in_data(manager this, const char *data);

manager _manager_init(manager this) {
    this->id = 0;
    this->name = new_string();
    this->registration_date = new_string();
    this->registered_by = new_string();
    this->_serialize = new_string();
    this->data = _manager_data;
    this->in_data = _manager_in_data;
    this->init = _manager_init;
    this->copy = _manager_copy;
    this->cmp = _manager_cmp;
    this->free = _manager_free;
    return this;
}

// 初始化 Manager 对象
manager new_manager() {
    manager this = (manager)malloc(sizeof(Manager));
    if (!this) {
        perror("Manager: this 指针分配失败");
        exit(EXIT_FAILURE);
    }
    _manager_init(this);
    return this;
}

manager __init_Manager(){
    return new_manager();
}

// 加载 Manager 数据
void load_manager(manager this, size_t id, const char *name, const char *registration_date, const char *registered_by) {
    this->id = id;
    this->name->assign_cstr(this->name, name);
    this->registration_date->assign_cstr(this->registration_date, registration_date);
    this->registered_by->assign_cstr(this->registered_by, registered_by);
}

// 序列化 Manager 数据
const char *_manager_data(manager this) {
    this->_serialize->clear(this->_serialize);
    size_t len = sizeof(this->id) + sizeof(size_t) + this->name->length(this->name) +
                 sizeof(size_t) + this->registration_date->length(this->registration_date) +
                 sizeof(size_t) + this->registered_by->length(this->registered_by);
    this->_serialize->append_n(this->_serialize, (const char*)&len, sizeof(len));
    this->_serialize->append_n(this->_serialize, (const char*)&this->id, sizeof(this->id));

    size_t str_len = this->name->length(this->name);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->name->c_str(this->name), str_len);

    str_len = this->registration_date->length(this->registration_date);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->registration_date->c_str(this->registration_date), str_len);

    str_len = this->registered_by->length(this->registered_by);
    this->_serialize->append_n(this->_serialize, (const char*)&str_len, sizeof(str_len));
    this->_serialize->append_n(this->_serialize, this->registered_by->c_str(this->registered_by), str_len);

    return this->_serialize->c_str(this->_serialize);
}

// 反序列化 Manager 数据
void _manager_in_data(manager this, const char *data) {
    size_t ptr = sizeof(size_t);
    size_t len = 0;
    memcpy(&this->id, data + ptr, sizeof(this->id));
    ptr += sizeof(this->id);

    size_t str_len = 0;
    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->name->assign_n(this->name, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->registration_date->assign_n(this->registration_date, data + ptr, str_len);
    ptr += str_len;

    memcpy(&str_len, data + ptr, sizeof(str_len));
    ptr += sizeof(str_len);
    this->registered_by->assign_n(this->registered_by, data + ptr, str_len);
    ptr += str_len;
}

// 释放 Manager 对象管理的内存
void _manager_free(manager this) {
    delete_string(this->name);
    delete_string(this->registration_date);
    delete_string(this->registered_by);
    delete_string(this->_serialize);
}

// 释放 Manager 对象本身
void free_manager(manager this) {
    if (!this) return;
    this->free(this);
    free(this);
}

// 复制 Manager 对象
manager _manager_copy(manager this, manager other) {
    this->id = other->id;
    this->name->assign_cstr(this->name, other->name->c_str(other->name));
    this->registration_date->assign_cstr(this->registration_date, other->registration_date->c_str(other->registration_date));
    this->registered_by->assign_cstr(this->registered_by, other->registered_by->c_str(other->registered_by));
    return this;
}

// 比较 Manager 对象
int _manager_cmp(manager this, manager other) {
    if (this->id != other->id) return this->id - other->id;
    return strcmp(this->name->c_str(this->name), other->name->c_str(other->name));
}