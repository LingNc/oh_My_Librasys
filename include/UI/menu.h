//打印界面菜单

#ifndef MENU_H
#define MENU_H

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <locale.h>
#include "screen.h"

// 显示标题
void display_title();

// 显示主菜单
Screen main_menu();

// 显示管理员菜单
Screen admin_menu();

// 显示图书管理信息
Screen book_manage_info();

// 显示学生管理信息
Screen stu_manage_info();

// 显示借阅记录
Screen borrow_records();

// 显示学生菜单
Screen stu_menu();

// // 显示搜索图书
// Screen search_books();

// 显示查看记录
Screen view_records();

// 显示借书
Screen borrow_book();

// 显示还书
Screen return_book();


// 显示关于信息
Screen about();

#endif // MENU_H