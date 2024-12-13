#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define HIGHT 24
#define WIDTH 80

WINDOW *display_background()
{
    int hight, width;
    hight = HIGHT;
    width = WIDTH;

    initscr();
    noecho();
    WINDOW *background = newwin(hight, width, 0, 0);

    mvprintw(4, 5, "                __         __  ___         __    _ __               _____              \n");
    mvprintw(5, 5, "         ____  / /_       /  |/  /_  __   / /   (_) /_  _________ _/ ___/__  _______   \n");
    mvprintw(6, 5, "        / __ \\/ __ \\     / /|_/ / / / /  / /   / / __ \\/ ___/ __ `/\\__ \\/ / / / ___/   \n");
    mvprintw(7, 5, "       / /_/ / / / /    / /  / / /_/ /  / /___/ / /_/ / /  / /_/ /___/ / /_/ (__  )    \n");
    mvprintw(8, 5, "       \\____/_/ /_/    /_/  /_/\\__, /  /_____/_/_.___/_/   \\__,_//____/\\__, /____/     \n");
    mvprintw(9, 5, "                              /____/                                  /____/           \n");
    refresh();

    return background;
}

void func(char *name)
{
    move(23, 0);
    clrtoeol();
    mvprintw(23, 0, "you select: %s", name);
}

 void display_main_menu()
{
    char *main_choices[] = {
        "MANAGER",
        "STUDENT",
        "Choice3",
        "ABOUT",
        "EXIT",
    };

    ITEM **main_items;  //主菜单的所有选项
    int n_main_choices; // 菜单选项数量
    MENU *main_menu;    //主菜单
    ITEM *cur_item;     //光标所指选项
    WINDOW *background, *win_menu; 

    //显示背景
    background = display_background();
    refresh();

    // 初始化选项item
    n_main_choices = ARRAY_SIZE(main_choices);
    main_items = (ITEM **)calloc(n_main_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_main_choices; i++)
    {
        main_items[i] = new_item(main_choices[i], "");
        // 为每个item绑定函数
        set_item_userptr(main_items[i], func);
    }
    main_items[n_main_choices] = (ITEM *)NULL;

    // item创建完毕,创建菜单
    main_menu = new_menu((ITEM **)main_items);

    win_menu = newwin(12, 20, 11, WIDTH/2);
    wborder(win_menu, ' ', ' ',' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(win_menu);

    // 将菜单绑定到main窗口
    set_menu_win(main_menu, win_menu);
    set_menu_sub(main_menu, derwin(win_menu, 9, 15, 1, 5));

    keypad(win_menu, true);

    set_menu_mark(main_menu, "*");
    set_menu_spacing(main_menu, 0, 2, 0);

    // 上传菜单
    mvprintw(LINES - 3, 0, "press <ENTER> chioce");
    mvprintw(LINES - 2, 0, "up and dowm, F1 out");

    post_menu(main_menu);
    wrefresh(win_menu);

    int input_c;        // 用户输入的按键,不能为char, 有些按键的宏大于128
    while ((input_c = wgetch(win_menu)) != KEY_BACKSPACE)
    {
        switch (input_c)
        {
        case KEY_DOWN:
            menu_driver(main_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(main_menu, REQ_UP_ITEM);
            break;
        case 10: // enter键
        {
            ITEM *cur;
            void (*p)(char *);
            cur = current_item(main_menu);
            // if(strcmp(item_name(cur_item), "exit" ) == 0)
            //     return;
            p = item_userptr(cur);
            p((char *)item_name(cur));
            post_menu(main_menu);
            refresh();
            break;
        }
        break;
        }
    }

    unpost_menu(main_menu);
    for (int i = 0; i < n_main_choices; ++i)
        free_item(main_items[i]);
    free_menu(main_menu);
    endwin();
}

int main()
{
    // 初始设置curses
    initscr();
    start_color();
    cbreak();
    noecho();
    init_pair(1, COLOR_CYAN, COLOR_BLUE);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    keypad(stdscr, true);

    display_main_menu();
    endwin();
}