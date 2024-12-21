#ifndef MENU_H
#define MENU_H
#include <wchar.h>
#include <stdbool.h>

void display_menu(int highlight, const wchar_t **choices, int n_choices);
bool handle_menu_input(int *highlight, int n_choices, int *choice);
void menu(const wchar_t **choices, void (**funcs)(void *), int n_choices, void *arg);

#endif // MENU_H