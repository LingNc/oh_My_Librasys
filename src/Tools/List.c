#include "Tools/List.h"

// 初始化列表
void InitList(List *plist) {
  assert(plist); // 确保plist不为空
  Data *ptr = (Data *)malloc(DEFAULT_CAPACITY * sizeof(Data)); // 分配内存
  if (NULL == ptr) {
    printf("malloc fail"); // 分配失败
    exit(-1); // 退出程序
  }
  plist->data = ptr; // 指向分配的内存
  plist->capacity = DEFAULT_CAPACITY; // 设置初始容量
  plist->size = 0; // 初始大小为0
}

// 销毁列表
void DestoryList(List *plist) {
  assert(plist); // 确保plist不为空
  free(plist->data); // 释放内存
  plist->data = NULL; // 置空指针
  plist->capacity = plist->size = 0; // 重置容量和大小
}

// 打印列表内容
void PrintList(const List *plist) {
  assert(plist); // 确保plist不为空
  for (int i = 0; i < plist->size; i++) { // 遍历所有元素
    if (0 == strcmp("char", plist->data[i].type)) { // 如果是char类型
      printf("<%s>%c ", plist->data[i].type, plist->data[i].data.c); // 打印char
      continue;
    }
    if (0 == strcmp("int", plist->data[i].type)) { // 如果是int类型
      printf("<%s>%d ", plist->data[i].type, plist->data[i].data.i); // 打印int
      continue;
    }
    if (0 == strcmp("double", plist->data[i].type)) { // 如果是double类型
      printf("<%s>%lf ", plist->data[i].type, plist->data[i].data.d); // 打印double
      continue;
    }
    if (0 == strcmp("size_t", plist->data[i].type)) { // 如果是size_t类型
      printf("<%s>%zu ", plist->data[i].type, plist->data[i].data.sz); // 打印size_t
      continue;
    }
    if (0 == strcmp("char*", plist->data[i].type)) { // 如果是char*类型
      printf("<%s>%s ", plist->data[i].type, plist->data[i].data.pc); // 打印char*
      continue;
    }
  }
  printf("\n");
}

// 检查并调整容量
int CheckCapacity(List *plist, size_t insertSize) {
  assert(plist && insertSize > 0); // 确保plist不为空且insertSize大于0
  if (insertSize <= plist->capacity - plist->size) { // 如果有足够的容量
    return 1; // 返回成功
  } else if (insertSize > 1) { // 如果需要插入的大小大于1
    Data *ptr = (Data *)realloc(plist->data, (plist->capacity + insertSize) * sizeof(Data)); // 重新分配内存
    if (NULL == ptr) { // 如果重新分配失败
      printf("realloc fail"); // 打印错误
      return 0; // 返回失败
    }
    plist->data = ptr; // 指向新的内存
    plist->capacity = plist->capacity + insertSize; // 更新容量
    return 1; // 返回成功
  } else { // 如果需要插入的大小为1
    Data *ptr = (Data *)realloc(plist->data, (plist->capacity * 2) * sizeof(Data)); // 将容量翻倍
    if (NULL == ptr) { // 如果重新分配失败
      printf("realloc fail"); // 打印错误
      return 0; // 返回失败
    }
    plist->data = ptr; // 指向新的内存
    plist->capacity = plist->capacity * 2; // 更新容量
    return 1; // 返回成功
  }
}

// 随机插入int类型数据
void RandomInsertIntData(List *plist, size_t count, size_t maxNum) {
  assert(plist && (maxNum + 1) < RAND_MAX); // 确保plist不为空且maxNum合法
  if (1 == CheckCapacity(plist, count)) { // 检查并调整容量
    for (int i = 0; i < count; i++) { // 循环插入count个数据
      plist->data[plist->size].data.i = rand() % (maxNum + 1); // 生成随机数
      plist->data[plist->size].type = "int"; // 设置类型
      plist->size++; // 增加大小
    }
  }
}

// 遍历并打印int类型数据
void TraverseIntData(const List *plist) {
  assert(plist); // 确保plist不为空
  for (int i = 0; i < plist->size; i++) { // 遍历所有元素
    if (0 == strcmp("int", plist->data[i].type)) { // 如果是int类型
      printf("%d ", plist->data[i].data.i); // 打印int数据
    }
  }
  printf("\n");
}

// 随机插入double类型数据
void RandomInsertDoubleData(List *plist, size_t count) {
  assert(plist); // 确保plist不为空
  if (1 == CheckCapacity(plist, count)) { // 检查并调整容量
    for (int i = 0; i < count; i++) { // 循环插入count个数据
      plist->data[plist->size].data.d = (double)rand() / RAND_MAX; // 生成随机double
      plist->data[plist->size].type = "double"; // 设置类型
      plist->size++; // 增加大小
    }
  }
}

// 遍历并打印double类型数据
void TraverseDoubleData(const List *plist) {
  assert(plist); // 确保plist不为空
  for (int i = 0; i < plist->size; i++) { // 遍历所有元素
    if (0 == strcmp("double", plist->data[i].type)) { // 如果是double类型
      printf("%lf ", plist->data[i].data.d); // 打印double数据
    }
  }
  printf("\n");
}

// 随机插入char类型数据
void RandomInsertCharData(List *plist, size_t count) {
  assert(plist); // 确保plist不为空
  if (1 == CheckCapacity(plist, count)) { // 检查并调整容量
    for (int i = 0; i < count; i++) { // 循环插入count个数据
      while (1) { // 生成一个大于32的随机字符
        int r = rand() % 127; // 生成随机数
        if (r > 32) { // 如果大于32
          plist->data[plist->size].data.c = r; // 存储字符
          break; // 退出循环
        }
      }
      plist->data[plist->size].type = "char"; // 设置类型
      plist->size++; // 增加大小
    }
  }
}

// 遍历并打印char类型数据
void TraverseCharData(const List *plist) {
  assert(plist); // 确保plist不为空
  for (int i = 0; i < plist->size; i++) { // 遍历所有元素
    if (0 == strcmp("char", plist->data[i].type)) { // 如果是char类型
      printf("%c ", plist->data[i].data.c); // 打印char数据
    }
  }
  printf("\n");
}

// 在列表末尾插入数据
void PushBack(List *plist, T data, const char *dataType) {
  assert(plist && dataType); // 确保plist和dataType不为空
  if (1 == CheckCapacity(plist, 1)) { // 检查并调整容量
    plist->data[plist->size].data = data; // 插入数据
    plist->data[plist->size].type = dataType; // 设置类型
    plist->size++; // 增加大小
  }
}

// 从列表末尾删除数据
void PopBack(List *plist) {
  assert(plist); // 确保plist不为空
  if (plist->size > 0) { // 如果列表不为空
    plist->size--; // 减少大小
  }
}

// 在列表开头插入数据
void PushFront(List *plist, T data, const char *dataType) {
  assert(plist && dataType); // 确保plist和dataType不为空
  if (1 == CheckCapacity(plist, 1)) { // 检查并调整容量
    for (int i = plist->size - 1; i >= 0; i--) { // 将所有元素向后移动一位
      plist->data[i + 1] = plist->data[i];
    }
    plist->data[0].data = data; // 插入数据到开头
    plist->data[0].type = dataType; // 设置类型
    plist->size++; // 增加大小
  }
}

// 从列表开头删除数据
void PopFront(List *plist) {
  assert(plist); // 确保plist不为空
  if (plist->size > 0) { // 如果列表不为空
    for (int i = 0; i < plist->size - 1; i++) { // 将所有元素向前移动一位
      plist->data[i] = plist->data[i + 1];
    }
    plist->size--; // 减少大小
  }
}

// 查找特定数据的位置
int FindListData(const List *plist, T target, const char *dataType) {
  assert(plist); // 确保plist不为空
  if (0 == strcmp("int", dataType)) { // 如果是int类型
    for (int i = 0; i < plist->size; i++) { // 遍历所有元素
      if (0 == strcmp("int", plist->data[i].type)) { // 如果是int类型
        if (target.i == plist->data[i].data.i) { // 如果找到匹配的数据
          return i; // 返回位置
        }
      }
    }
    return -1; // 没找到
  }
  if (0 == strcmp("char*", dataType)) { // 如果是char*类型
    for (int i = 0; i < plist->size; i++) { // 遍历所有元素
      if (0 == strcmp("char*", plist->data[i].type)) { // 如果是char*类型
        if (0 == strcmp(target.pc, plist->data[i].data.pc)) { // 如果找到匹配的数据
          return i; // 返回位置
        }
      }
    }
    return -1; // 没找到
  }
  if (0 == strcmp("char", dataType)) { // 如果是char类型
    for (int i = 0; i < plist->size; i++) { // 遍历所有元素
      if (0 == strcmp("char", plist->data[i].type)) { // 如果是char类型
        if (target.c == plist->data[i].data.c) { // 如果找到匹配的数据
          return i; // 返回位置
        }
      }
    }
    return -1; // 没找到
  }
  if (0 == strcmp("double", dataType)) { // 如果是double类型
    for (int i = 0; i < plist->size; i++) { // 遍历所有元素
      if (0 == strcmp("double", plist->data[i].type)) { // 如果是double类型
        if (target.d == plist->data[i].data.d) { // 如果找到匹配的数据
          return i; // 返回位置
        }
      }
    }
    return -1; // 没找到
  }
  if (0 == strcmp("size_t", dataType)) { // 如果是size_t类型
    for (int i = 0; i < plist->size; i++) { // 遍历所有元素
      if (0 == strcmp("size_t", plist->data[i].type)) { // 如果是size_t类型
        if (target.sz == plist->data[i].data.sz) { // 如果找到匹配的数据
          return i; // 返回位置
        }
      }
    }
    return -1; // 没找到
  }
  return -1; // 数据类型不匹配
}

// 在指定位置插入数据
void PosInsert(List *plist, int pos, T data, const char *dataType) {
  assert(plist && dataType && pos >= 0); // 确保plist、dataType不为空且pos非负
  if (1 == CheckCapacity(plist, 1)) { // 检查并调整容量
    for (int i = plist->size - 1; i >= pos; i--) { // 将从pos开始的元素向后移动一位
      plist->data[i + 1] = plist->data[i];
    }
    plist->data[pos].data = data; // 插入数据到pos位置
    plist->data[pos].type = dataType; // 设置类型
    plist->size++; // 增加大小
  }
}

// 在指定位置删除数据
void PosErase(List *plist, int pos) {
  assert(plist && pos >= 0); // 确保plist不为空且pos非负
  if (plist->size > 0) { // 如果列表不为空
    for (int i = pos; i < plist->size - 1; i++) { // 将从pos开始的元素向前移动一位
      plist->data[i] = plist->data[i + 1];
    }
    plist->size--; // 减少大小
  }
}

// 获取列表的大小
size_t ListSize(List *plist) {
  assert(plist); // 确保plist不为空
  return plist->size; // 返回大小
}

// 获取列表的容量
size_t ListCapacity(List *plist) {
  assert(plist); // 确保plist不为空
  return plist->capacity; // 返回容量
}