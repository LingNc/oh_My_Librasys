#include "Tools/List.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// 创建一个新的链表
static List *_list_new(void);

// 创建新节点
static List_node *_list_node_new(const char *data);

// 初始化链表的函数指针
static List *_List_init_func(List *self);

// 销毁链表
static void _list_destroy(List *self);

// 在链表尾部添加节点
static List_node *_push_back(List *self, List_node *current);

// 从链表尾部移除节点
static void *_pop_back(List *self);

// 从链表头部移除节点
static void *_pop_front(List *self);

// 在链表头部添加节点
static List_node *_push_front(List *self, List_node *current);

// 按索引访问链表中的节点
static List_node *_at(List *self, int pos);

// 从指定位置删除数据
static void _erase(List *self, int pos);

// 移除链表中的指定节点
static void _remove(List *self, List_node *current);

static void _list_node_destroy(List_node *self);
// 销毁链表
static void _delete(List *self);

// 在指定位置插入数据
static void _insert(List *self, int pos, const char *data);

// 打印链表中的所有元素
static void _print(const List *self);

// 获取链表中元素的数量
static size_t _size(List *self);

// 获取链表的容量
static size_t _capacity(List *self);

// 查找链表中的数据
static int _find(const List *self, const char *target);

static List *_list_new(void)
{
  List *self;
  assert(self = malloc(sizeof(List)));
  _List_init_func(self);

  return self;
}

// 创建新节点
static List_node *_list_node_new(const char *data)
{
  List_node *new_node = (List_node *)malloc(sizeof(List_node));
  assert(new_node);
  new_node->val = strdup(data); // 复制字符串
  new_node->prev = NULL;
  new_node->next = NULL;
  return new_node;
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
  // 设置创建新节点的函数
  self->list_node_new = _list_node_new;

  // 初始化数据成员
  self->head = NULL; // 头节点指针初始化为NULL
  self->tail = NULL; // 尾节点指针初始化为NULL
  self->len = 0;     // 链表长度初始化为0

  return self;
}

// 在指定位置插入数据
static void _insert(List *self, int pos, const char *data)
{
  assert(self && pos >= 0 && pos <= self->len);
  List_node *new_node = _list_node_new(data);

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

static List_node *_push_back(List *self, List_node *current)
{
  assert(current);

  if (self->len)
  {                             // 链表不为空
    current->prev = self->tail; // 设置新节点的前指针为当前尾节点
    current->next = NULL;       // 新节点的后指针为空
    self->tail->next = current; // 当前尾节点的后指针指向新节点
    self->tail = current;       // 更新尾指针为新节点
  }
  else
  {                                       // 链表为空
    self->head = self->tail = current;    // 头尾指针都指向新节点
    current->prev = current->next = NULL; // 新节点的前后指针都为空
  }

  ++self->len; // 增加链表长度
  return current;
}

static void *_pop_back(List *self)
{
  assert(self->len);

  List_node *current = self->tail; // 获取尾节点

  if (--self->len)
  {                                            // 链表长度减一，如果仍不为零
    (self->tail = current->prev)->next = NULL; // 更新尾指针并断开新尾节点的后指针
  }
  else
  {                                 // 链表为空
    self->tail = self->head = NULL; // 头尾指针都置空
  }

  current->next = current->prev = NULL; // 断开删除节点的前后指针
  _list_node_destroy(current);
}

static void *_pop_front(List *self)
{
  assert(self->len);

  List_node *current = self->head; // 获取头节点

  if (--self->len)
  {                                            // 链表长度减一，如果仍不为零
    (self->head = current->next)->prev = NULL; // 更新头指针并断开新头节点的前指针
  }
  else
  {                                 // 链表为空
    self->head = self->tail = NULL; // 头尾指针都置空
  }

  current->next = current->prev = NULL; // 断开删除节点的前后指针
  _list_node_destroy(current);
}

static List_node *_push_front(List *self, List_node *current)
{
  assert(current);

  if (self->len)
  {                             // 链表不为空
    current->next = self->head; // 设置新节点的后指针为当前头节点
    current->prev = NULL;       // 新节点的前指针为空
    self->head->prev = current; // 当前头节点的前指针指向新节点
    self->head = current;       // 更新头指针为新节点
  }
  else
  {                                       // 链表为空
    self->head = self->tail = current;    // 头尾指针都指向新节点
    current->prev = current->next = NULL; // 新节点的前后指针都为空
  }

  ++self->len; // 增加链表长度
  return current;
}

static void _list_node_destroy(List_node *self)
{
  assert(self);
  if(self->val){
    free(self->val);
  }
  free(self); // 释放节点本身
}

static void _list_destroy(List *self)
{
  assert(self);

  List_node *current = self->head;
  while (current != NULL)
  {
    List_node *next = current->next;
    _list_node_destroy(current);
    current = next;
  }

  self->head = NULL;
  self->tail = NULL;
  self->len = 0;
}

// 销毁链表
static void _delete(List *self)
{
  assert(self);
  List_node *current = self->head;
  while (current != NULL)
  {
    List_node *next = current->next;
    _list_node_destroy(current); // 释放节点
    current = next;
  }
  self->head = NULL;
  self->tail = NULL;
  self->len = 0;
}

// 查找链表中的数据
static int _find(const List *self, const char *target)
{
  assert(self && self->head);
  List_node *current = self->head;
  int index = 0;
  while (current != NULL)
  {
    if (!strcmp(current->val, target))
      ;
    {
      return index;
    }
    current = current->next;
    index++;
  }
  return -1;
}

static List_node *_at(List *self, int index)
{
  assert(index >= 0 && index < self->len); // 索引在范围内
  List_node *current = self->head;         // 获取第一个节点
  while (index--)
    current = current->next; // 移动到指定索引的节点
  return current;            // 返回该节点
}

// 从指定位置删除数据
static void _erase(List *self, int pos)
{
  List_node *current = _at(self, pos);

  if (current->prev)                     // 如果有前驱节点
    current->prev->next = current->next; // 前驱节点的后指针指向当前节点的后继
  else
    self->head = current->next; // 没有前驱，表示是头节点，更新头指针

  if (current->next)                     // 如果有后继节点
    current->next->prev = current->prev; // 后继节点的前指针指向当前节点的前驱
  else
    self->tail = current->prev; // 没有后继，表示是尾节点，更新尾指针

  _list_node_destroy(current);
  self->len--;
}

static void _remove(List *self, List_node *current)
{
  if (current->prev)                     // 如果有前驱节点
    current->prev->next = current->next; // 前驱节点的后指针指向当前节点的后继
  else
    self->head = current->next; // 没有前驱，表示是头节点，更新头指针

  if (current->next)                     // 如果有后继节点
    current->next->prev = current->prev; // 后继节点的前指针指向当前节点的前驱
  else
    self->tail = current->prev; // 没有后继，表示是尾节点，更新尾指针

  _list_node_destroy(current); // 释放节点内存
  --self->len;                 // 减少链表长度
}

// 打印链表中的所有元素
static void _print(const List *self)
{
  assert(self && self->head);
  List_node *current = self->head;
  while (current != NULL)
  {
    printf("Value: %s\n", current->val);
    current = current->next;
  }
  printf("\n");
}

// 获取链表中元素的数量
static size_t _size(List *self)
{
  return (size_t)self->len;
}
