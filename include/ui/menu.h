#ifndef _menu_h
#define _menu_h
#include <wchar.h>
#include <stdbool.h>
#include "DataBase/DataBase.h"


extern dataBase studentDb, managerDb;

// void display_menu(int highlight, const wchar_t **choices, int n_choices);
// bool handle_menu_input(int *highlight, int n_choices, int *choice);
void menu(const wchar_t **choices, void (**funcs)(void **), int n_choices, void **arg);
void clear_screen();
void login(void **arg);

#endif // MENU_H