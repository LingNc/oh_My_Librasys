#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "utils.h"

#define HIGHT 24
#define WIDTH 80


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))    //只能在主函数用,指针退化,无法得出

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



Screen main_menu()
{
    char *main_choices[] = {
        "管理员入口",
        "学生入口",
        "选项",
        "关于",
        "退出",
    };
    // Choice main_choices[] = {
    //     {"管理员入口", ADMIN_MENU},
    //     {"学生入口", STUDENT_MENU},
    //     {"关于", ABOUT},
    //     {"退出", EXIT},
    // };
    // 清空屏幕
    clear();
    refresh();

    ITEM **main_items;  // 主菜单的所有选项
    int n_main_choices; // 菜单选项数量
    MENU *main_menu;    // 主菜单
    ITEM *cur_item;     // 光标所指选项
    WINDOW *background, *win_menu;

    // 显示背景
    background = display_background();
    refresh();

    n_main_choices = ARRAY_SIZE(main_choices);
    main_items = creat_items(main_choices, n_main_choices);

    // item创建完毕,创建菜单
    main_menu = new_menu((ITEM **)main_items);
    win_menu = creat_win(12, 20, 11, WIDTH / 2);
    wborder(win_menu, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(win_menu);

    init_menu_in_win(main_menu, win_menu, 9, 15, 1, 5);

    // 上传菜单
    mvprintw(LINES - 3, 0, "按 <ENTER> 选择");
    mvprintw(LINES - 2, 0, "上下选择, 退格键退出");

    refresh_menu(main_menu, win_menu);

    Screen next_screen;
    int input_c; // 用户输入的按键,不能为char, 有些按键的宏大于128
    while ((input_c = wgetch(win_menu)) != KEY_F(1))
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
            const char *chioce_name = item_name(cur);
            if (strcmp(chioce_name, "退出") == 0)
            {
                // 返回主界面
                destroy_menu(main_menu, main_items, win_menu);
                next_screen = EXIT;
                return next_screen;
            }

            mvwprintw(stdscr, 1,1,"ok");
            // void (*p)(char *);
            // if(strcmp(item_name(cur_item), "exit" ) == 0)
            //     return;
            // p = item_userptr(cur);
            // p((char *)item_name(cur));
            // post_menu(main_menu);
            // refresh();
        }
        break;
        }

    }
}