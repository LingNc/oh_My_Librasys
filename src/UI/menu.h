#ifndef MENU_H
#define MENU_H

#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <locale.h>


void main_menu_display();
 Screen (*_handler)(Stack *stack);

WINDOW *display_background();

void display_manager_menu();


#endif