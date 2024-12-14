#ifndef UTILS_H
#define UTILS_H

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <locale.h>
#include <stdlib.h>


void init_all();

//创建item组,用来与菜单绑定
ITEM** creat_items(char** choices, int n_choices);

//创建窗口,返回窗口指针
WINDOW* creat_win(int hight, int width, int y, int x);

void init_menu_in_win(MENU* menu, WINDOW* win, int hight, int width, int y, int x);

void refresh_menu(MENU* menu, WINDOW* win);

void destroy_menu(MENU* menu, ITEM** items, WINDOW* menu_win);

#endif