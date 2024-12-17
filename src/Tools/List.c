#include "Tools/List.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>




List *new_List(void)
{
  List *self;
  if (!(self = LIST_MALLOC(sizeof(List))))
    return NULL;      // 分配内存失败，返回NULL
  self->head = NULL;  // 初始化头指针为空
  self->tail = NULL;  // 初始化尾指针为空
  self->free = NULL;  // 初始化free函数指针为空
  self->match = NULL; // 初始化match函数指针为空
  self->len = 0;      // 初始化长度为0
  return self;
}

void destroy_list(List *self)
{
  unsigned int len = self->len;
  List_node *next;
  List_node *curr = self->head;

  while (len--)
  { // 遍历所有节点
    next = curr->next;
    if (self->free)
      self->free(curr->val); // 如果有free函数，释放节点值
    LIST_FREE(curr);         // 释放当前节点内存
    curr = next;
  }

  LIST_FREE(self); // 释放列表结构体内存
}

List_node *push_back(List *self, List_node *node)
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

List_node *pop_back(List *self)
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

List_node *pop_front(List *self)
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

List_node *push_front(List *self, List_node *node)
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

List_node *find_node(List *self, void *val)
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

List_node *get_node_at(List *self, int index)
{
  assert(index >= 0 && index < self->len); // 索引在范围内
  List_node *node = self->head;            // 获取第一个节点
  while (index--)
    node = node->next; // 移动到指定索引的节点
  return node;         // 返回该节点
}

void remove_node(List *self, List_node *node)
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

int CheckCapacity(List *self, size_t insertSize)
{
  assert(self && insertSize > 0);
  if (insertSize <= self->capacity - self->size)
  {
    return 1;
  }
  else if (insertSize > 1)
  {
    Data *ptr = (Data *)realloc(
        self->data, (self->capacity + insertSize) * sizeof(Data));
    if (NULL == ptr)
    {
      printf("realloc fail");
      return 0;
    }
    self->data = ptr;
    self->capacity = self->capacity + insertSize;
    return 1;
  }
  else
  {
    Data *ptr = (Data *)realloc(
        self->data, (self->capacity * 2) * sizeof(Data));
    if (NULL == ptr)
    {
      printf("realloc fail");
      return 0;
    }
    self->data = ptr;
    self->capacity = self->capacity * 2;
    return 1;
  }
}



void PrintList(const List *self)
{
  assert(self);
  for (int i = 0; i < self->size; i++)
  {
    if (0 == strcmp("char", self->data[i].type))
    {
      printf("<%s>%c ", self->data[i].type, self->data[i].data.c);
    }
    else if (0 == strcmp("int", self->data[i].type))
    {
      printf("<%s>%d ", self->data[i].type, self->data[i].data.i);
    }
    else if (0 == strcmp("double", self->data[i].type))
    {
      printf("<%s>%lf ", self->data[i].type, self->data[i].data.d);
    }
    else if (0 == strcmp("size_t", self->data[i].type))
    {
      printf("<%s>%zu ", self->data[i].type, self->data[i].data.sz);
    }
    else if (0 == strcmp("char*", self->data[i].type))
    {
      printf("<%s>%s ", self->data[i].type, self->data[i].data.pc);
    }
  }
  printf("\n");
}









void InitList(List *self)
{
  assert(self);
  Data *ptr = (Data *)malloc(DEFAULT_CAPACITY * sizeof(Data));
  if (NULL == ptr)
  {
    printf("malloc fail");
    exit(-1);
  }
  self->data = ptr;
  self->capacity = DEFAULT_CAPACITY;
  self->size = 0;
}
void DestoryList(List *self)
{
  assert(self);
  free(self->data);
  self->data = NULL;
  self->capacity = self->size = 0;
}





void PushBack(List *self, T data, const char *dataType)
{
  assert(self && dataType);
  if (1 == CheckCapacity(self, 1))
  {
    self->data[self->size].data = data;
    self->data[self->size].type = dataType;
    self->size++;
  }
}
void PopBack(List *self)
{
  assert(self);
  if (self->size > 0)
  {
    self->size--;
  }
}
void PushFront(List *self, T data, const char *dataType)
{
  assert(self && dataType);
  if (1 == CheckCapacity(self, 1))
  {
    for (int i = self->size - 1; i >= 0; i--)
    {
      self->data[i + 1] = self->data[i];
    }
    self->data[0].data = data;
    self->data[0].type = dataType;
    self->size++;
  }
}
void PopFront(List *self)
{
  assert(self);
  if (self->size > 0)
  {
    for (int i = 0; i < self->size - 1; i++)
    {
      self->data[i] = self->data[i + 1];
    }
    self->size--;
  }
}
int FindListData(const List *self, T target, const char *dataType)
{
  assert(self);
  if (0 == strcmp("int", dataType))
  {
    for (int i = 0; i < self->size; i++)
    {
      if (0 == strcmp("int", self->data[i].type))
      {
        if (target.i == self->data[i].data.i)
        {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("char*", dataType))
  {
    for (int i = 0; i < self->size; i++)
    {
      if (0 == strcmp("char*", self->data[i].type))
      {
        if (0 == strcmp(target.pc, self->data[i].data.pc))
        {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("char", dataType))
  {
    for (int i = 0; i < self->size; i++)
    {
      if (0 == strcmp("char", self->data[i].type))
      {
        if (target.c == self->data[i].data.c)
        {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("double", dataType))
  {
    for (int i = 0; i < self->size; i++)
    {
      if (0 == strcmp("double", self->data[i].type))
      {
        if (target.d == self->data[i].data.d)
        {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("size_t", dataType))
  {
    for (int i = 0; i < self->size; i++)
    {
      if (0 == strcmp("size_t", self->data[i].type))
      {
        if (target.sz == self->data[i].data.sz)
        {
          return i;
        }
      }
    }
    return -1;
  }
  return -1;
}
void PosInsert(List *self, int pos, T data, const char *dataType)
{
  assert(self && dataType && pos >= 0);
  if (1 == CheckCapacity(self, 1))
  {
    for (int i = self->size - 1; i >= pos; i--)
    {
      self->data[i + 1] = self->data[i];
    }
    self->data[pos].data = data;
    self->data[pos].type = dataType;
    self->size++;
  }
}
void PosErase(List *self, int pos)
{
  assert(self && pos >= 0);
  if (self->size > 0)
  {
    for (int i = pos; i < self->size - 1; i++)
    {
      self->data[i] = self->data[i + 1];
    }
    self->size--;
  }
}
size_t ListSize(List *self)
{
  assert(self);
  return self->size;
}
size_t ListCapacity(List *self)
{
  assert(self);
  return self->capacity;
}