#ifndef LIST_H
#define LIST_H

#include <ncursesw/ncurses.h>
#include "utils.h"

#define LIST_SIZE 150      // 列表项总数
#define VISIBLE_ROWS 20   // 可见行数
#define PAD_HEIGHT (LIST_SIZE + 1)  // pad高度
#define PAD_WIDTH (ISBN_WIDTH + BOOKNAME_WIDTH + AUTHOR_WIDTH + PUBLISHER_WIDTH + NUM_WIDTH + 10)     // pad宽度
#define ISBN_WIDTH 30   // ISBN
#define BOOKNAME_WIDTH 20    // 书名
#define AUTHOR_WIDTH 10  // 作者
#define PUBLISHER_WIDTH 40  // 出版社
#define NUM_WIDTH 4   // 总数量

// 列表项结构体
typedef struct {
    char isbn[ISBN_WIDTH];
    char name[BOOKNAME_WIDTH];
    char author[AUTHOR_WIDTH];
    char publisher[PUBLISHER_WIDTH];
    int num;
} Book;

void list();

#endif
