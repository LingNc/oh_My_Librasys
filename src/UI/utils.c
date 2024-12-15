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
    update_terminal_size();
    if (terminal.width < 87 || terminal.height < 24)
    {
        endwin();
        fprintf(stderr, "终端尺寸过小，请调整终端窗口大小。\n");
        exit(1);
    }
}

ITEM** creat_items(char** choices, int n_choices)
{
    ITEM **items;
    items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_choices; i++)
    {
        items[i] = new_item(choices[i], "");
    }
    items[n_choices] = (ITEM*)NULL;
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
void resize_main_menu(MENU *menu, WINDOW **win)
{
    unpost_menu(menu);  // 取消发布菜单
    if (*win != NULL)
        delwin(*win);
    *win = creat_win(12, 20, 14, (terminal.width - 20) / 2);
    keypad(*win, TRUE);
    init_menu_in_win(menu, *win, 9, 15, 1, 5);
    wborder(*win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    post_menu(menu);
    wrefresh(menu_sub(menu));  // 刷新子窗口
    wrefresh(*win);
    refresh();  // 刷新整个屏幕
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

