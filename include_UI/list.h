#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include "uiBook.h"
#include "uiStudent.h"
#include "DataBase/DataBase.h"

extern dataBase bookDb,studentDb;
extern uibook *bookArray ;
extern uistudent *studentArray;

#define LIST_SIZE 99      // 列表项总数
#define VISIBLE_ROWS 20   // 可见行数
#define PAD_HEIGHT (LIST_SIZE + 1)  // pad高度
#define PAD_WIDTH (ISBN_WIDTH + BOOKNAME_WIDTH + AUTHOR_WIDTH + PUBLISHER_WIDTH + NUM_WIDTH + 10)     // pad宽度
#define ISBN_WIDTH 30   // ISBN
#define BOOKNAME_WIDTH 20    // 书名
#define AUTHOR_WIDTH 20  // 作者
#define PUBLISHER_WIDTH 40  // 出版社
#define NUM_WIDTH 4   // 总数量
//#define MAX_AVAILABLE 60   // 最大可接阅数

// 图书列表项结构体
// typedef struct {
//     char ISBN[ISBN_WIDTH];
//     char name[BOOKNAME_WIDTH];
//     char author[AUTHOR_WIDTH];
//     char publisher[PUBLISHER_WIDTH];
//     int num;
// } uiBook;

// 图书列表项结构体
// typedef struct {
//     size_t id;
//     char ISBN[ISBN_WIDTH];
//     char name[BOOKNAME_WIDTH];
//     char author[AUTHOR_WIDTH];
//     char publisher[PUBLISHER_WIDTH];
//     char time[20];
//     int status;
// } uiBook;

// // 学生列表项结构体
// typedef struct {
//     char id[30];          // 学生ID
//     char name[20];        // 学生姓名
//     char class[20];  // 班级
//     char department[40];  // 学院
//     int borrowedCount;     //以借阅数量
//     char borrowedDate[20];          // 借阅日期
//     char return_date[20];            // 归还日期
//     uiBook *books[MAX_AVAILABLE];  // 借阅图书的信息
// } uiStudent;

typedef struct {
    char info1[30];
    char info2[20];
    char info3[20];
    char info4[40];
}Inputinfo;

typedef struct {
    size_t all;
    size_t acceess;
    char name[100];
    char date[20];
}MenuInfo;//菜单栏上的信息

typedef struct {
    char field1[20];
    char field2[20];
    char field3[20];
    char field4[20];
}Infoname;//菜单栏上的信息名

void book_list(uiBook** books);
void stu_list(uiStudent** student);
void stu_borrow_list();

#endif
