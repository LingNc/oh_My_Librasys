#include <assert.h> // IWYU pragma: export
#include <stdio.h>  // IWYU pragma: export
#include <stdlib.h> // IWYU pragma: export
#include <string.h> // IWYU pragma: export
#include <time.h>   // IWYU pragma: export
#include <Tools/String.h>
// T Union: 用于存储不同类型的数据，节省内存

#define List(TYPE) new_List(#TYPE)

#ifdef LIST_CONFIG_H
#define _STR(x) #x
#define STR(x) _STR(x)
#include STR(LIST_CONFIG_H)
#undef _STR
#undef STR
#endif

#ifndef LIST_MALLOC
#define LIST_MALLOC malloc
#endif

#ifndef LIST_FREE
#define LIST_FREE free
#endif
typedef union
{
  char c;    // 存储 char 类型
  int i;     // 存储 int 类型
  double d;  // 存储 double 类型
  size_t sz; // 存储 size_t 类型
  char *pc;  // 存储 char* 类型
} T;

// Data结构体: 存储一个数据元素及其类型
typedef struct
{
  T data;           // 存储数据的联合体
  const char *type; // 数据的类型（例如"int", "char", "double"等）
} Data;

typedef struct list_node
{
  struct list_node *prev; // 指向前一个节点
  struct list_node *next; // 指向后一个节点
  void *val;              // 节点存储的值
             // 内部数据类型名
  string _typename;

} List_node;

// List结构体: 动态数组，支持多类型数据
typedef struct List
{
  // 链表的头节点指针
  List_node *head;

  // 链表的尾节点指针
  List_node *tail;

  // 序列化字符串
  string _serialize;

  // 单个元素大小
  size_t _itemSize;

  // 已分配的内存大小
  size_t _allocatedSize;

  // 当前元素数量
  size_t _size;

  // 链表的长度（节点数量）
  unsigned int len;

  // 释放节点值的函数指针，用于自定义释放节点值的逻辑
  void (*free)(void *val);

  // 匹配节点值的函数指针，用于自定义比较节点值的逻辑
  int (*match)(void *a, void *b);

  // 存储数据的数组
  void *data;

  // 当前存储的元素数量
  size_t size;

  // 数组的容量（最大可存储的元素数量）
  size_t capacity;

  // 数据复制函数指针，用于自定义数据复制逻辑
  void (*copy)(void *dest, void *src);

  // 数据释放函数指针，用于自定义数据释放逻辑
  void (*free)(void *data);

  // 数据比较函数指针，用于自定义数据比较逻辑
  int (*cmp)(void *a, void *b);

  // 内部数据类型名，用于标识存储的数据类型
  string _typename;

  // 内部数据指针，用于存储内部数据
  void *_data;

  // 内部基准数据函数，可能用于某些内部操作
  void *_base;

  // 初始化元素的函数指针，用于自定义初始化逻辑
  void (*_init_item)(void *self);

  // 释放单个元素的函数指针，用于自定义释放单个元素的逻辑
  void (*_free_item)(void *itemAddr);

  // 复制单个元素的函数指针，用于自定义复制单个元素的逻辑
  void (*_copy_item)(void *dest, void *src);

  // 比较两个元素的函数指针，用于自定义比较两个元素的逻辑
  int (*_cmp_item)(void *item1, void *item2);

  // 用于元素序列化的函数指针，将元素转换为字符串
  const char *(*_data_item)(void *);

  // 用于元素反序列化的函数指针，将字符串转换为元素
  void (*_in_data_item)(void *, void *);

  // 创建新链表的函数指针
  List *(*list_new)(void);

  // 销毁链表的函数指针，释放所有资源
  void (*list_destroy)(List *self);

  // 在链表尾部添加节点的函数指针
  List_node *(*push_back)(List *self, List_node *node);

  // 在链表头部添加节点的函数指针
  List_node *(*push_front)(List *self, List_node *node);

  // 查找值为val的节点的函数指针
  List_node *(*find)(List *self, void *val);

  // 获取索引为index的节点的函数指针
  List_node *(*at)(List *self, int index);

  // 从尾部移除节点并返回的函数指针
  List_node *(*pop_back)(List *self);

  // 从头部移除节点并返回的函数指针
  List_node *(*pop_front)(List *self);

  // 从链表中移除指定节点的函数指针
  void (*remove)(List *self, List_node *node);

  // 销毁链表的函数指针，释放所有资源
  void (*delete)(List *self);

  // 在指定位置插入数据的函数指针
  void (*insert)(List *self, int pos, T data, const char *dataType);

  // 从指定位置删除数据的函数指针
  void (*erase)(List *self, int pos);

  // 打印List中所有元素的函数指针
  void (*print)(const List *self);

  // 获取List中元素数量的函数指针
  size_t (*size)(List *self);

  // 查找List中数据的函数指针
  int (*find_data)(const List *self, T target, const char *dataType);

  // 创建新节点，存储值val的函数指针
  List_node *(*list_node_new)(void *val);
} List;

// #define DEFAULT_CAPACITY 8 // 默认容量为8

// // 类型安全的宏来简化PushBack操作
// #define mPushBack(pLIST, DATA)                    \
//   _Generic(DATA,                                  \
//       char: PushBack(pLIST, (T)DATA, "char"),     \
//       int: PushBack(pLIST, (T)DATA, "int"),       \
//       double: PushBack(pLIST, (T)DATA, "double"), \
//       size_t: PushBack(pLIST, (T)DATA, "size_t"), \
//       char *: PushBack(pLIST, (T)DATA, "char*"),  \
//       default: PushBack(pLIST, (T)DATA, "int"))

// // 类型安全的宏来简化PushFront操作
// #define mPushFront(pLIST, DATA)                    \
//   _Generic(DATA,                                   \
//       char: PushFront(pLIST, (T)DATA, "char"),     \
//       int: PushFront(pLIST, (T)DATA, "int"),       \
//       double: PushFront(pLIST, (T)DATA, "double"), \
//       size_t: PushFront(pLIST, (T)DATA, "size_t"), \
//       char *: PushFront(pLIST, (T)DATA, "char*"),  \
//       default: PushFront(pLIST, (T)DATA, "int"))

// // 类型安全的宏来简化FindListData操作
// #define mFindListData(pLIST, DATA)                    \
//   _Generic(DATA,                                      \
//       char: FindListData(pLIST, (T)DATA, "char"),     \
//       int: FindListData(pLIST, (T)DATA, "int"),       \
//       double: FindListData(pLIST, (T)DATA, "double"), \
//       size_t: FindListData(pLIST, (T)DATA, "size_t"), \
//       char *: FindListData(pLIST, (T)DATA, "char*"),  \
//       default: FindListData(pLIST, (T)DATA, "int"))

// // 类型安全的宏来简化PosInsert操作
// #define mPosInsert(pLIST, POS, DATA)                    \
//   _Generic(DATA,                                        \
//       char: PosInsert(pLIST, POS, (T)DATA, "char"),     \
//       int: PosInsert(pLIST, POS, (T)DATA, "int"),       \
//       double: PosInsert(pLIST, POS, (T)DATA, "double"), \
//       size_t: PosInsert(pLIST, POS, (T)DATA, "size_t"), \
//       char *: PosInsert(pLIST, POS, (T)DATA, "char*"),  \
//       default: PosInsert(pLIST, POS, (T)DATA, "int"))

// #define mPrint(pLIST, type, data)             \
//   _Generic((data),                            \
//       char: printf("<%s>%c ", type, data),    \
//       int: printf("<%s>%d ", type, data),     \
//       double: printf("<%s>%lf ", type, data), \
//       size_t: printf("<%s>%zu ", type, data), \
//       char *: printf("<%s>%s ", type, data))
