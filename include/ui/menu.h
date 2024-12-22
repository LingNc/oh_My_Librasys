#ifndef _menu_h
#define _menu_h
#include <wchar.h>
#include <stdbool.h>
#include "DataBase/DataBase.h"

extern dataBase studentDb, managerDb;

// 菜单组件
void menu(int n_choices, const wchar_t **choices, void (**funcs)(void *), void **arg);
// 登陆组件
void login(void *arg);

#endif // MENU_H