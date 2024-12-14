#include "utils.h"

void init_all()
{
    // 设置本地化以支持多字节字符
    setlocale(LC_ALL, "");
    // 初始设置curses
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, true);
}

ITEM** creat_items(char** choices, int n_choices)
{
    ITEM **items;
    items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_choices; i++)
    {
        items[i] = new_item(choices[i], "");
    }
    items[n_choices] =(ITEM**)NULL;
    return items;
}

// void set_func(ITEM** items, int n_chioces, )
// {
    
// }

WINDOW* creat_win(int hight, int width, int y, int x)
{
    WINDOW* win;
    win = newwin(hight, width, y, x);
    box(win, 0, 0);
    return win;
}

void init_menu_in_win(MENU* menu, WINDOW* win, int hight, int width, int y, int x)
{
    init_pair(1, COLOR_CYAN, COLOR_WHITE);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, hight, width, y, x));
    keypad(win, true);
    set_menu_mark(menu, "*");
    set_menu_fore(menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(menu, COLOR_PAIR(2));
    set_menu_spacing(menu, 0, 2, 0);
}

void refresh_menu(MENU* menu, WINDOW* win)
{
    post_menu(menu);
    wrefresh(win);
}

void destroy_menu(MENU* menu, ITEM** items, WINDOW* menu_win)
{
    int n_choices = item_count(menu);
    unpost_menu(menu);
    for (int i = 0; i < n_choices + 1; i++)
        free_item(items[i]);
    free_menu(menu);
    free(items);
    delwin(menu_win);
}

