#include "Tools/List.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// 创建一个新的链表
static List *_list_new(void);

// 销毁链表
static void _list_destroy(List *self);

// 在链表尾部添加节点
static List_node *_push_back(List *self, List_node *node);

// 从链表尾部移除节点
static List_node *_pop_back(List *self);

// 从链表头部移除节点
static List_node *_pop_front(List *self);

// 在链表头部添加节点
static List_node *_push_front(List *self, List_node *node);

// 查找链表中的节点
static List_node *_find(List *self, void *val);

// 按索引访问链表中的节点
static List_node *_at(List *self, int index);

// 移除链表中的指定节点
static void _remove(List *self, List_node *node);

// 销毁链表
static void _delete(List *self);

// 在指定位置插入数据
static void _insert(List *self, int pos, void *data, const char *dataType);

// 从指定位置删除数据
static void _erase(List *self, int pos);

// 打印链表中的所有元素
static void _print(const List *self);

// 获取链表中元素的数量
static size_t _size(List *self);

// 获取链表的容量
static size_t _capacity(List *self);

// 查找链表中的数据
static int _find_data(const List *self, void *target, const char *dataType);

// 创建新节点
static List_node *_list_node_new(void *data, const char *dataType);

// 初始化链表的函数指针
static List *_List_init_func(List *self);

static List *_list_new(void)
{
  List *self;
  if (!(self = LIST_MALLOC(sizeof(List))))
    return NULL; // 分配内存失败，返回NULL
  _List_init_func(self);

  return self;
}

static List *_List_init_func(List *self)
{
  // 初始化链表的函数指针
  self->list_new = _list_new;
  // 设置销毁链表的函数
  self->list_destroy = _list_destroy;
  // 设置尾部添加节点的函数
  self->push_back = _push_back;
  // 设置头部添加节点的函数
  self->push_front = _push_front;
  // 设置查找节点的函数
  self->find = _find;
  // 设置按索引访问节点的函数
  self->at = _at;
  // 设置从尾部移除节点的函数
  self->pop_back = _pop_back;
  // 设置从头部移除节点的函数
  self->pop_front = _pop_front;
  // 设置移除指定节点的函数
  self->remove = _remove;
  // 设置销毁链表的函数
  self->delete = _delete;
  // 设置插入数据的函数
  self->insert = _insert;
  // 设置删除数据的函数
  self->erase = _erase;
  // 设置打印链表的函数
  self->print = _print;
  // 设置获取链表大小的函数
  self->size = _size;
  // 设置查找数据的函数
  self->find_data = _find_data;
  // 设置创建新节点的函数
  self->list_node_new = _list_node_new;

  // 初始化数据成员
  self->head = NULL;        // 头节点指针初始化为NULL
  self->tail = NULL;        // 尾节点指针初始化为NULL
  self->len = 0;            // 链表长度初始化为0
  self->data = NULL;        // 数据数组指针初始化为NULL
  self->size = 0;           // 当前存储的元素数量初始化为0
  self->free = NULL;        // 初始化free函数指针为空
  self->match = NULL;       // 初始化match函数指针为空
  self->capacity = 0;       // 数组容量初始化为0
  self->_typename = "List"; // 设置内部数据类型名为"List"

  return self;
}

static void _list_destroy(List *self) {
    if (self == NULL) return;

    List_node *current = self->head;
    while (current != NULL) {
        List_node *next = current->next;

        // 释放节点值
        if (self->free != NULL) {
            self->free(current->val); // 调用自定义释放函数
        }

        // 释放节点本身
        free(current->_typename); // 释放类型名字符串
        free(current);            // 释放节点

        current = next;
    }

    self->head = NULL;
    self->tail = NULL;
    self->len = 0;
}

static List_node *_push_back(List *self, List_node *node)
{
  if (!node)
    return NULL; // 节点为空，返回NULL

  if (self->len)
  {                          // 链表不为空
    node->prev = self->tail; // 设置新节点的前指针为当前尾节点
    node->next = NULL;       // 新节点的后指针为空
    self->tail->next = node; // 当前尾节点的后指针指向新节点
    self->tail = node;       // 更新尾指针为新节点
  }
  else
  {                                 // 链表为空
    self->head = self->tail = node; // 头尾指针都指向新节点
    node->prev = node->next = NULL; // 新节点的前后指针都为空
  }

  ++self->len; // 增加链表长度
  return node;
}

static List_node *_pop_back(List *self)
{
  if (!self->len)
    return NULL; // 链表为空，返回NULL

  List_node *node = self->tail; // 获取尾节点

  if (--self->len)
  {                                         // 链表长度减一，如果仍不为零
    (self->tail = node->prev)->next = NULL; // 更新尾指针并断开新尾节点的后指针
  }
  else
  {                                 // 链表为空
    self->tail = self->head = NULL; // 头尾指针都置空
  }

  node->next = node->prev = NULL; // 断开删除节点的前后指针
  return node;
}

static List_node *_pop_front(List *self)
{
  if (!self->len)
    return NULL; // 链表为空，返回NULL

  List_node *node = self->head; // 获取头节点

  if (--self->len)
  {                                         // 链表长度减一，如果仍不为零
    (self->head = node->next)->prev = NULL; // 更新头指针并断开新头节点的前指针
  }
  else
  {                                 // 链表为空
    self->head = self->tail = NULL; // 头尾指针都置空
  }

  node->next = node->prev = NULL; // 断开删除节点的前后指针
  return node;
}

static List_node *_push_front(List *self, List_node *node)
{
  if (!node)
    return NULL; // 节点为空，返回NULL

  if (self->len)
  {                          // 链表不为空
    node->next = self->head; // 设置新节点的后指针为当前头节点
    node->prev = NULL;       // 新节点的前指针为空
    self->head->prev = node; // 当前头节点的前指针指向新节点
    self->head = node;       // 更新头指针为新节点
  }
  else
  {                                 // 链表为空
    self->head = self->tail = node; // 头尾指针都指向新节点
    node->prev = node->next = NULL; // 新节点的前后指针都为空
  }

  ++self->len; // 增加链表长度
  return node;
}

static List_node *_find(List *self, void *val)
{
  List_node *node = self->head;

  while (node)
  { // 遍历链表
    if (self->match)
    { // 如果有匹配函数
      if (self->match(val, node->val))
      {              // 调用匹配函数检查值
        return node; // 返回匹配的节点
      }
    }
    else
    { // 没有匹配函数
      if (val == node->val)
      {              // 直接比较值
        return node; // 返回匹配的节点
      }
    }
    node = node->next;
  }
  return NULL; // 没有找到匹配节点
}

static List_node *_at(List *self, int index)
{
  assert(index >= 0 && index < self->len); // 索引在范围内
  List_node *node = self->head;            // 获取第一个节点
  while (index--)
    node = node->next; // 移动到指定索引的节点
  return node;         // 返回该节点
}

static void _remove(List *self, List_node *node)
{
  if (node->prev)                  // 如果有前驱节点
    node->prev->next = node->next; // 前驱节点的后指针指向当前节点的后继
  else
    self->head = node->next; // 没有前驱，表示是头节点，更新头指针

  if (node->next)                  // 如果有后继节点
    node->next->prev = node->prev; // 后继节点的前指针指向当前节点的前驱
  else
    self->tail = node->prev; // 没有后继，表示是尾节点，更新尾指针

  if (self->free)          // 如果有free函数
    self->free(node->val); // 释放节点值

  LIST_FREE(node); // 释放节点内存
  --self->len;     // 减少链表长度
}

// 销毁链表
static void _delete(List *self)
{
  if (self == NULL)
    return;
  List_node *current = self->head;
  while (current != NULL)
  {
    List_node *next = current->next;
    if (self->free != NULL)
    {
      self->free(current->val); // 调用自定义释放函数
    }
    free(current->_typename); // 释放类型名字符串
    free(current);            // 释放节点
    current = next;
  }
  self->head = NULL;
  self->tail = NULL;
  self->len = 0;
}

// 在指定位置插入数据
static void _insert(List *self, int pos, void *data, const char *dataType)
{
  assert(self != NULL && pos >= 0 && pos <= self->len);
  List_node *new_node = _list_node_new(data, dataType);

  assert(new_node != NULL);
  if (pos == 0)
  { // 插入到头部
    new_node->next = self->head;
    if (self->head != NULL)
    {
      self->head->prev = new_node;
    }
    self->head = new_node;
    if (self->tail == NULL)
    {
      self->tail = new_node;
    }
  }
  else if (pos == self->len)
  { // 插入到尾部
    self->tail->next = new_node;
    new_node->prev = self->tail;
    self->tail = new_node;
  }
  else
  { // 插入到中间
    List_node *current = self->head;
    for (int i = 0; i < pos; i++)
    {
      current = current->next;
    }
    new_node->next = current;
    new_node->prev = current->prev;
    current->prev->next = new_node;
    current->prev = new_node;
  }
  self->len++;
}

// 从指定位置删除数据
static void _erase(List *self, int pos)
{
  if (self == NULL || pos < 0 || pos >= self->len)
    return; // 无效位置
  List_node *current = self->head;
  for (int i = 0; i < pos; i++)
  {
    current = current->next;
  }
  if (current->prev != NULL)
  {
    current->prev->next = current->next;
  }
  else
  {
    self->head = current->next;
  }
  if (current->next != NULL)
  {
    current->next->prev = current->prev;
  }
  else
  {
    self->tail = current->prev;
  }
  if (self->free != NULL)
  {
    self->free(current->val); // 调用自定义释放函数
  }
  free(current->_typename); // 释放类型名字符串
  free(current);            // 释放节点
  self->len--;
}

// 打印链表中的所有元素
static void _print(const List *self)
{
  if (self == NULL || self->head == NULL)
    return;
  List_node *current = self->head;
  while (current != NULL)
  {
    printf("Value: %p, Type: %s\n", current->val, current->_typename);
    current = current->next;
  }
}

// 获取链表中元素的数量
static size_t _size(List *self)
{
  return (size_t)self->len;
}

// 查找链表中的数据
static int _find_data(const List *self, void *target, const char *dataType)
{
  if (self == NULL || self->head == NULL)
    return -1;
  List_node *current = self->head;
  int index = 0;
  while (current != NULL)
  {
    if (self->match != NULL && self->match(current->val, target))
    {
      return index;
    }
    current = current->next;
    index++;
  }
  return -1;
}

// 创建新节点
static List_node *_list_node_new(void *data, const char *dataType)
{
  List_node *node;
  if (!(node = LIST_MALLOC(sizeof(List_node))))
  {
    return NULL; // 内存分配失败，返回 NULL
  }
  node->_typename = dataType;
  node->val = data;  // 将传入的值赋给节点的 val 字段
  node->prev = NULL; // 初始化前驱指针为 NULL
  node->next = NULL; // 初始化后继指针为 NULL
  return node;       // 返回新创建的节点
}
