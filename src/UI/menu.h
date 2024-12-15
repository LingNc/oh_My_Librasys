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


WINDOW *display_background();

Screen main_menu();


#endif