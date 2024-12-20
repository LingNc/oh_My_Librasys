#ifndef UTILS_H
#define UTILS_H

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <locale.h>
#include <stdlib.h>
#include "screen.h"
#include "list.h"
#include "window.h"


#define HEIGHTLIGHT 1
#define FONT 2
#define PAD_HEIGHT_LIGHT 3
#define FOCUSED 4
#define UNHEIGHT 5

void init_all();

//创建item组,用来与菜单绑定
ITEM** creat_items(char** choices, int n_choices);

//终端尺寸
typedef struct {
    int width;
    int height;
} TerminalSize;
extern TerminalSize terminal;

//计算终端的大小
void update_terminal_size();


//创建窗口,返回窗口指针
WINDOW* creat_win(int hight, int width, int y, int x);

void init_menu_in_win(MENU* menu, WINDOW* win, int hight, int width, int y, int x);

void refresh_menu(MENU* menu, WINDOW* win);
void resize_main_menu(MENU *menu, WINDOW **win);

void destroy_menu(MENU* menu, ITEM** items, WINDOW* menu_win);

void resize_menu(MENU *menu, WINDOW **win);

void refresh_win(WINDOW** win);

void print_list(uiBook** books, WINDOW* pad, int i);

void print_student_list(uiStudent** students, WINDOW* pad, int i);

void update_window_focus(WINDOW *win, bool focused);

void print_info(WINDOW* info_win, Infoname* infoname, MenuInfo* info);




#endif