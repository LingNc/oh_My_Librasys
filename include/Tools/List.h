#include <assert.h> // IWYU pragma: export
#include <stdio.h>  // IWYU pragma: export
#include <stdlib.h> // IWYU pragma: export
#include <string.h> // IWYU pragma: export
#include <time.h>   // IWYU pragma: export

// T Union: 用于存储不同类型的数据，节省内存
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

// List结构体: 动态数组，支持多类型数据
typedef struct
{
  Data *data;      // 存储数据的数组
  size_t size;     // 当前存储的元素数量
  size_t capacity; // 数组的容量
  // 函数指针，用于数据复制、释放和比较
  void (*copy)(void *dest, void *src);
  void (*free)(void *data);
  int (*cmp)(void *a, void *b);

  // 接口函数
  void (*push_back)(struct List *this, void *data);
  void (*pop_back)(struct List *this);
  int (*find)(struct List *this, void *data);
  void (*destroy)(struct List *this);
  void (*print)(const struct List *this);
  size_t (*size)(const struct List *this);
  size_t (*capacity)(const struct List *this);
} List;

#define DEFAULT_CAPACITY 8 // 默认容量为8

// 初始化List
void InitList(List *plist);

// 销毁List
void DestoryList(List *plist);

// 打印List中的所有元素
void PrintList(const List *plist);

// 检查是否有足够容量插入指定数量的元素
int CheckCapacity(List *plist, size_t insertSize);

// 随机插入int类型数据
void RandomInsertIntData(List *plist, size_t count, size_t maxNum);

// 遍历并打印int类型数据
void TraverseIntData(const List *plist);

// 随机插入double类型数据
void RandomInsertDoubleData(List *plist, size_t count);

// 遍历并打印double类型数据
void TraverseDoubleData(const List *plist);

// 随机插入char类型数据
void RandomInsertCharData(List *plist, size_t count);

// 遍历并打印char类型数据
void TraverseCharData(const List *plist);

// 向List末尾添加数据
void PushBack(List *plist, T data, const char *dataType);

// 类型安全的宏来简化PushBack操作
#define mPushBack(pLIST, DATA)                    \
  _Generic(DATA,                                  \
      char: PushBack(pLIST, (T)DATA, "char"),     \
      int: PushBack(pLIST, (T)DATA, "int"),       \
      double: PushBack(pLIST, (T)DATA, "double"), \
      size_t: PushBack(pLIST, (T)DATA, "size_t"), \
      char *: PushBack(pLIST, (T)DATA, "char*"),  \
      default: PushBack(pLIST, (T)DATA, "int"))

// 从List末尾删除数据
void PopBack(List *plist);

// 向List开头添加数据
void PushFront(List *plist, T data, const char *dataType);

// 类型安全的宏来简化PushFront操作
#define mPushFront(pLIST, DATA)                    \
  _Generic(DATA,                                   \
      char: PushFront(pLIST, (T)DATA, "char"),     \
      int: PushFront(pLIST, (T)DATA, "int"),       \
      double: PushFront(pLIST, (T)DATA, "double"), \
      size_t: PushFront(pLIST, (T)DATA, "size_t"), \
      char *: PushFront(pLIST, (T)DATA, "char*"),  \
      default: PushFront(pLIST, (T)DATA, "int"))

// 从List开头删除数据
void PopFront(List *plist);

// 在List中查找数据的索引
int FindListData(const List *plist, T target, const char *dataType);

// 类型安全的宏来简化FindListData操作
#define mFindListData(pLIST, DATA)                    \
  _Generic(DATA,                                      \
      char: FindListData(pLIST, (T)DATA, "char"),     \
      int: FindListData(pLIST, (T)DATA, "int"),       \
      double: FindListData(pLIST, (T)DATA, "double"), \
      size_t: FindListData(pLIST, (T)DATA, "size_t"), \
      char *: FindListData(pLIST, (T)DATA, "char*"),  \
      default: FindListData(pLIST, (T)DATA, "int"))

// 在指定位置插入数据
void PosInsert(List *plist, int pos, T data, const char *dataType);

// 类型安全的宏来简化PosInsert操作
#define mPosInsert(pLIST, POS, DATA)                    \
  _Generic(DATA,                                        \
      char: PosInsert(pLIST, POS, (T)DATA, "char"),     \
      int: PosInsert(pLIST, POS, (T)DATA, "int"),       \
      double: PosInsert(pLIST, POS, (T)DATA, "double"), \
      size_t: PosInsert(pLIST, POS, (T)DATA, "size_t"), \
      char *: PosInsert(pLIST, POS, (T)DATA, "char*"),  \
      default: PosInsert(pLIST, POS, (T)DATA, "int"))

// 从指定位置删除数据
void PosErase(List *plist, int pos);

// 获取List中元素的数量
size_t ListSize(List *plist);

// 获取List的容量
size_t ListCapacity(List *plist);

// 排序List（此函数尚未实现）
/*
void SortList(List *plist);
*/
