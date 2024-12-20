
#ifndef MENU_H
#define MENU_H
#include <wchar.h>

void display_menu(int highlight,const wchar_t **choices,int n_choices);
size_t handle_menu_input(int *highlight, int n_choices, int *choice);

#endif // MENU_H