#include <ncursesw/ncurses.h>
#include <ncursesw/menu.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

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

void func(char *name);
 void display_main_menu();


void display_manager_menu()
{
    char *manage_choices[] = {
        "书籍管理",
        "学生管理",
        "信息检索",
        "系统设置",
        "退出",
    };

    // 清空屏幕
    clear();
    refresh();

    // 初始化菜单项
    ITEM **manag_items;
    int n_manag_choices;
    n_manag_choices = ARRAY_SIZE(manage_choices);
    manag_items = (ITEM **)calloc(n_manag_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_manag_choices; i++)
    {
        manag_items[i] = new_item(manage_choices[i], "");
    }
    manag_items[n_manag_choices] = NULL;

    // 创建菜单
    MENU *manag_menu = new_menu((ITEM **)manag_items);

    // 设置菜单为横向排列
    set_menu_format(manag_menu, 1, n_manag_choices);

    // 计算每个选项的最大长度
    int max_len = 0;
    for (int i = 0; i < n_manag_choices; i++)
    {
        int len = strlen(manage_choices[i]);
        if (len > max_len)
            max_len = len;
    }

    // 计算选项之间的间隔，使其平均分布
    int spacing = (COLS - n_manag_choices * max_len) / (n_manag_choices + 1);
    if (spacing < 1) spacing = 1; // 确保间隔至少为1

    set_menu_spacing(manag_menu, 0, 0, spacing); // 设置选项间距

    // 调整菜单窗口的位置，在屏幕底部
    int win_height = 3;
    int win_width = COLS;
    int win_starty = LINES - win_height;
    int win_startx = 0;
    WINDOW *manag_menu_win = newwin(win_height, win_width, win_starty, win_startx);
    keypad(manag_menu_win, TRUE);

    // 设置菜单窗口边框
    box(manag_menu_win, 0, 0);
    wrefresh(manag_menu_win);

    // 将菜单绑定到窗口
    set_menu_win(manag_menu, manag_menu_win);
    set_menu_sub(manag_menu, derwin(manag_menu_win, 1, win_width - 2, 1, 1));

    // 设置菜单选项
    set_menu_mark(manag_menu, "*");
    set_menu_fore(manag_menu, A_REVERSE);
    set_menu_back(manag_menu, A_NORMAL);

    // 显示菜单
    post_menu(manag_menu);
    wrefresh(manag_menu_win);

    // 处理用户输入
    int c;
    while((c = wgetch(manag_menu_win)) != KEY_BACKSPACE)
    {
        switch(c)
        {
            case KEY_LEFT:
                menu_driver(manag_menu, REQ_LEFT_ITEM);
                break;
            case KEY_RIGHT:
                menu_driver(manag_menu, REQ_RIGHT_ITEM);
                break;
            case 10: // Enter 键
            {
                ITEM *cur = current_item(manag_menu);
                if (strcmp(item_name(cur), "退出") == 0)
                {
                    // 返回主界面
                    unpost_menu(manag_menu);
                    free_menu(manag_menu);
                    for(int i = 0; i < n_manag_choices; ++i)
                        free_item(manag_items[i]);
                    free(manag_items);
                    delwin(manag_menu_win);
                    display_main_menu(); // 调用主界面函数
                    return;
                }
                else
                {
                    
                }
                break;
            }
            default:
                break;
        }
        wrefresh(manag_menu_win);
    }

    // 清理
    unpost_menu(manag_menu);
    free_menu(manag_menu);
    for(int i = 0; i < n_manag_choices; ++i)
        free_item(manag_items[i]);
    free(manag_items);
    delwin(manag_menu_win);
}

 void display_main_menu()
{
    char *main_choices[] = {
        "管理员入口",
        "学生入口",
        "选项",
        "关于",
        "退出",
    };

    // 清空屏幕
    clear();
    refresh();

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
    set_menu_fore(main_menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(main_menu, COLOR_PAIR(2));
    set_menu_spacing(main_menu, 0, 2, 0);

    // 上传菜单
    mvprintw(LINES - 3, 0, "按 <ENTER> 选择");
    mvprintw(LINES - 2, 0, "上下选择, 退格键退出");

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
            ITEM *cur = current_item(main_menu);
            if (strcmp(item_name(cur), "退出") == 0)
            {
                // 返回主界面
                unpost_menu(main_menu);
                for(int i = 0; i < n_main_choices; i++)
                    free_item(main_items[i]);
                free(main_items);
                delwin(win_menu);
                return;
            }
            void (*p)(char *);
            
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

void func(char *name)
{
    if (strcmp(name, "管理员入口") == 0)
    {
        display_manager_menu();
    }
    else
    {
        move(23, 0);
        clrtoeol();
        mvprintw(23, 0, "你选择了: %s", name);
        refresh();
    }
}
int main()
{
    // 设置本地化以支持多字节字符
    setlocale(LC_ALL, "");
    // 初始设置curses
    initscr();
    start_color();
    cbreak();
    noecho();
    init_pair(1, COLOR_CYAN, COLOR_WHITE);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    keypad(stdscr, true);

    display_main_menu();
    //display_manager_menu();


    endwin();
}