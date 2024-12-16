#include "list.h"
 
void InitList(List *plist) {
  assert(plist);
  Data *ptr = (Data *)malloc(DEFAULT_CAPACITY * sizeof(Data));
  if (NULL == ptr) {
    printf("malloc fail");
    exit(-1);
  }
  plist->data = ptr;
  plist->capacity = DEFAULT_CAPACITY;
  plist->size = 0;
}
void DestoryList(List *plist) {
  assert(plist);
  free(plist->data);
  plist->data = NULL;
  plist->capacity = plist->size = 0;
}
void PrintList(const List *plist) {
  assert(plist);
  for (int i = 0; i < plist->size; i++) {
    if (0 == strcmp("char", plist->data[i].type)) {
      printf("<%s>%c ", plist->data[i].type, plist->data[i].data.c);
      continue;
    }
    if (0 == strcmp("int", plist->data[i].type)) {
      printf("<%s>%d ", plist->data[i].type, plist->data[i].data.i);
      continue;
    }
    if (0 == strcmp("double", plist->data[i].type)) {
      printf("<%s>%lf ", plist->data[i].type, plist->data[i].data.d);
      continue;
    }
    if (0 == strcmp("size_t", plist->data[i].type)) {
      printf("<%s>%zu ", plist->data[i].type,
             plist->data[i].data.sz);
      continue;
    }
    if (0 == strcmp("char*", plist->data[i].type)) {
      printf("<%s>%s ", plist->data[i].type, plist->data[i].data.pc);
      continue;
    }
  }
  printf("\n");
}
int CheckCapacity(List *plist, size_t insertSize) {
  assert(plist && insertSize > 0);
  if (insertSize <= plist->capacity - plist->size) {
    return 1;
  } else if (insertSize > 1) {
    Data *ptr = (Data *)realloc(
        plist->data, (plist->capacity + insertSize) * sizeof(Data));
    if (NULL == ptr) {
      printf("realloc fail");
      return 0;
    }
    plist->data = ptr;
    plist->capacity = plist->capacity + insertSize;
    return 1;
  } else {
    Data *ptr = (Data *)realloc(
        plist->data, (plist->capacity * 2) * sizeof(Data));
    if (NULL == ptr) {
      printf("realloc fail");
      return 0;
    }
    plist->data = ptr;
    plist->capacity = plist->capacity * 2;
    return 1;
  }
}
void RandomInsertIntData(List *plist, size_t count, size_t maxNum) {
  assert(plist && (maxNum + 1) < RAND_MAX);
  if (1 == CheckCapacity(plist, count)) {
    for (int i = 0; i < count; i++) {
      plist->data[plist->size].data.i = rand() % (maxNum + 1);
      plist->data[plist->size].type = "int";
      plist->size++;
    }
  }
}
void TraverseIntData(const List *plist) {
  assert(plist);
  for (int i = 0; i < plist->size; i++) {
    if (0 == strcmp("int", plist->data[i].type)) {
      printf("%d ", plist->data[i].data.i);
    }
  }
  printf("\n");
}
void RandomInsertDoubleData(List *plist, size_t count) {
  assert(plist);
  if (1 == CheckCapacity(plist, count)) {
    for (int i = 0; i < count; i++) {
      plist->data[plist->size].data.d = (double)rand() / RAND_MAX;
      plist->data[plist->size].type = "double";
      plist->size++;
    }
  }
}
void TraverseDoubleData(const List *plist) {
  assert(plist);
  for (int i = 0; i < plist->size; i++) {
    if (0 == strcmp("double", plist->data[i].type)) {
      printf("%lf ", plist->data[i].data.d);
    }
  }
  printf("\n");
}
void RandomInsertCharData(List *plist, size_t count) {
  assert(plist);
  if (1 == CheckCapacity(plist, count)) {
    for (int i = 0; i < count; i++) {
      while (1) {
        int r = rand() % 127;
        if (r > 32) {
          plist->data[plist->size].data.c = r;
          break;
        }
      }
      plist->data[plist->size].type = "char";
      plist->size++;
    }
  }
}
void TraverseCharData(const List *plist) {
  assert(plist);
  for (int i = 0; i < plist->size; i++) {
    if (0 == strcmp("char", plist->data[i].type)) {
      printf("%c ", plist->data[i].data.c);
    }
  }
  printf("\n");
}
void PushBack(List *plist, T data, const char *dataType) {
  assert(plist && dataType);
  if (1 == CheckCapacity(plist, 1)) {
    plist->data[plist->size].data = data;
    plist->data[plist->size].type = dataType;
    plist->size++;
  }
}
void PopBack(List *plist) {
  assert(plist);
  if (plist->size > 0) {
    plist->size--;
  }
}
void PushFront(List *plist, T data, const char *dataType) {
  assert(plist && dataType);
  if (1 == CheckCapacity(plist, 1)) {
    for (int i = plist->size - 1; i >= 0; i--) {
      plist->data[i + 1] = plist->data[i];
    }
    plist->data[0].data = data;
    plist->data[0].type = dataType;
    plist->size++;
  }
}
void PopFront(List *plist) {
  assert(plist);
  if (plist->size > 0) {
    for (int i = 0; i < plist->size - 1; i++) {
      plist->data[i] = plist->data[i + 1];
    }
    plist->size--;
  }
}
int FindListData(const List *plist, T target, const char *dataType) {
  assert(plist);
  if (0 == strcmp("int", dataType)) {
    for (int i = 0; i < plist->size; i++) {
      if (0 == strcmp("int", plist->data[i].type)) {
        if (target.i == plist->data[i].data.i) {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("char*", dataType)) {
    for (int i = 0; i < plist->size; i++) {
      if (0 == strcmp("char*", plist->data[i].type)) {
        if (0 == strcmp(target.pc, plist->data[i].data.pc)) {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("char", dataType)) {
    for (int i = 0; i < plist->size; i++) {
      if (0 == strcmp("char", plist->data[i].type)) {
        if (target.c == plist->data[i].data.c) {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("double", dataType)) {
    for (int i = 0; i < plist->size; i++) {
      if (0 == strcmp("double", plist->data[i].type)) {
        if (target.d == plist->data[i].data.d) {
          return i;
        }
      }
    }
    return -1;
  }
  if (0 == strcmp("size_t", dataType)) {
    for (int i = 0; i < plist->size; i++) {
      if (0 == strcmp("size_t", plist->data[i].type)) {
        if (target.sz == plist->data[i].data.sz) {
          return i;
        }
      }
    }
    return -1;
  }
  return -1;
}
void PosInsert(List *plist, int pos, T data, const char *dataType) {
  assert(plist && dataType && pos >= 0);
  if (1 == CheckCapacity(plist, 1)) {
    for (int i = plist->size - 1; i >= pos; i--) {
      plist->data[i + 1] = plist->data[i];
    }
    plist->data[pos].data = data;
    plist->data[pos].type = dataType;
    plist->size++;
  }
}
void PosErase(List *plist, int pos) {
  assert(plist && pos >= 0);
  if (plist->size > 0) {
    for (int i = pos; i < plist->size - 1; i++) {
      plist->data[i] = plist->data[i + 1];
    }
    plist->size--;
  }
}
size_t ListSize(List *plist) {
  assert(plist);
  return plist->size;
}
size_t ListCapacity(List *plist) {
  assert(plist);
  return plist->capacity;
}