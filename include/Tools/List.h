#include <assert.h> // IWYU pragma: export
#include <stdio.h>  // IWYU pragma: export
#include <stdlib.h> // IWYU pragma: export
#include <string.h> // IWYU pragma: export
#include <time.h>   // IWYU pragma: export
#include <Tools/String.h>
// T Union: 用于存储不同类型的数据，节省内存

#define List(TYPE) new_List(#TYPE)

typedef struct list_node
{
  struct list_node *prev; // 指向前一个节点
  struct list_node *next; // 指向后一个节点
  char *val;              // 节点存储的值
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

  // 链表的长度（节点数量）
  unsigned int len;

  // 创建新链表的函数指针
  List *(*list_new)(void);

  // 销毁链表的函数指针，释放所有资源
  void (*list_destroy)(List *self);

  // 在链表尾部添加节点的函数指针
  List_node *(*push_back)(List *self, List_node *node);

  // 在链表头部添加节点的函数指针
  List_node *(*push_front)(List *self, List_node *node);

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

  // 从指定位置删除数据的函数指针
  void (*erase)(List *self, int pos);

  // 打印List中所有元素的函数指针
  void (*print)(const List *self);

  // 获取List中元素数量的函数指针
  size_t (*size)(List *self);

  // 在指定位置插入数据的函数指针
  void (*insert)(List *self, int pos,  const char *data);

  // 查找值为val的节点的函数指针
  List_node *(*find)(List *self, const char *val);

  // 创建新节点，存储值val的函数指针
  List_node *(*list_node_new)(const char *data);
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
