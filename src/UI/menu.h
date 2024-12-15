//打印界面菜单

#ifndef MENU_H
#define MENU_H

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <locale.h>
#include "screen.h"

//显示界面函数
// void display_main_menu(void);
// void display_manager_menu();


void display_title();

Screen main_menu();
Screen admin_menu();


#endif