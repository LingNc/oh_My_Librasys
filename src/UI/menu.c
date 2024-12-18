#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "UI/utils.h"
#include "UI/list.h"
#include "UI/test.h"
#include "function.h"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))    //只能在主函数用,指针退化,无法得出

// uiBook books[20] = ;
// uiStudent students[20];


void display_title()
{
    update_terminal_size();
    int width = (terminal.width - 87) / 2;

    // init_pair(8, COLOR_RED, COLOR_BLACK);
    // init_pair(9, COLOR_YELLOW, COLOR_BLACK);
    // init_pair(10, COLOR_GREEN, COLOR_BLACK);
    // init_pair(11, COLOR_CYAN, COLOR_BLACK);
    // init_pair(12, COLOR_BLUE, COLOR_BLACK);
    // init_pair(13, COLOR_MAGENTA, COLOR_BLACK);
    // const char *text[] = {
    //     "                __         __  ___         __    _ __               _____              ",
    //     "         ____  / /_       /  |/  /_  __   / /   (_) /_  _________ _/ ___/__  _______   ",
    //     "        / __ \\/ __ \\     / /|_/ / / / /  / /   / / __ \\/ ___/ __ `/\\__ \\/ / / / ___/   ",
    //     "       / /_/ / / / /    / /  / / /_/ /  / /___/ / /_/ / /  / /_/ /___/ / /_/ (__  )    ",
    //     "       \\____/_/ /_/    /_/  /_/\\__, /  /_____/_/_.___/_/   \\__,_//____/\\__, /____/     ",
    //     "                              /____/                                  /____/           "
    // };
    mvprintw(4, width, "                __         __  ___         __    _ __               _____              ");
    mvprintw(5, width, "         ____  / /_       /  |/  /_  __   / /   (_) /_  _________ _/ ___/__  _______   ");
    mvprintw(6, width, "        / __ \\/ __ \\     / /|_/ / / / /  / /   / / __ \\/ ___/ __ `/\\__ \\/ / / / ___/   ");
    mvprintw(7, width, "       / /_/ / / / /    / /  / / /_/ /  / /___/ / /_/ / /  / /_/ /___/ / /_/ (__  )    ");
    mvprintw(8, width, "       \\____/_/ /_/    /_/  /_/\\__, /  /_____/_/_.___/_/   \\__,_//____/\\__, /____/     ");
    mvprintw(9, width, "                              /____/                                  /____/           ");
    
    //    // 打印彩色渐变文本
    // for (int i = 0; i < 6; i++) {
    //     attron(COLOR_PAIR((i % 6) + 1));
    //     mvprintw(start_row + i, start_col, "%s", text[i]);
    //     attroff(COLOR_PAIR((i % 6) + 1));
    // }
    mvprintw(LINES - 3, 0, "按 <ENTER> 选择");
    mvprintw(LINES - 2, 0, "上下选择, 退格键退出");
    refresh();
}

//主菜单
Screen main_menu()
{
    char *main_choices[] = {
        "管理员入口",
        "学生入口",
        "选项",
        "关于",
        "退出",
    };
    clear();
    refresh();
    display_title();
    refresh();

    int n_choices = ARRAY_SIZE(main_choices);      // 菜单选项数量
    ITEM **items = creat_items(main_choices, n_choices);// 将字符串初始话为主菜单的选项

    MENU *menu = new_menu((ITEM **)items);    // item创建完毕,创建菜单
    WINDOW *menu_win = creat_win(12, 20, 14, (terminal.width - 20) / 2);
    wborder(menu_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    init_menu_in_win(menu, menu_win, 9, 15, 1, 5);
    refresh_menu(menu, menu_win);   //刷新menu

    Screen next_screen;
    int input_c; // 用户输入的按键,不能为char, 有些按键的宏大于128
    while ((input_c = wgetch(menu_win)) != KEY_BACKSPACE)
    {
        switch (input_c)
        {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case KEY_RESIZE:
            clear();
            display_title();
            resize_main_menu(menu, &menu_win); 
            refresh();
            break; 
        case 10: // enter键
        {
            ITEM *cur = current_item(menu);
            const char *chioce_name = item_name(cur);
            if (strcmp(chioce_name, "退出") == 0)
            {
                // 返回主界面
                destroy_menu(menu, items, menu_win);
                next_screen = EXIT;
                return next_screen;
            }else if(strcmp(chioce_name, "管理员入口") == 0)
            {
                return ADMIN_MENU;
            }else if(strcmp(chioce_name, "学生入口") == 0)
            {
                return STUDENT_MENU;
            }else if(strcmp(chioce_name, "关于") == 0)
            {
                return ABOUT;
            }

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

//用户菜单界面
Screen admin_menu()
{
    char *admin_choices[] = {
        "图书管理",
        "学生管理",
        "借书记录管理",
        "退出登录",
    };
    // 清空屏幕
    clear();
    refresh();

    ITEM **items;  
    int n_choices; 

    n_choices = ARRAY_SIZE(admin_choices);
    items = creat_items(admin_choices, n_choices);

    MENU *menu = new_menu((ITEM **)items);
    WINDOW *menu_win = creat_win(terminal.height - 2, terminal.width / 4, 1, 1);
    keypad(menu_win, TRUE);
    

    init_menu_in_win(menu, menu_win,terminal.height - 4, terminal.width / 4 - 2, 1, 1);

    refresh_menu(menu, menu_win);

    WINDOW *info = creat_win(terminal.height - 2, terminal.width - (terminal.width / 4) - 3, 1, terminal.width / 4 + 2);
    wrefresh(info);
    
    Screen next_screen;
    int input_c; // 用户输入的按键,不能为char, 有些按键的宏大于128
    while ((input_c = wgetch(menu_win)) != KEY_F(1))
    {
        switch (input_c)
        {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case KEY_RESIZE:
            clear();
            resize_menu(menu, &menu_win);
            refresh_win(&info);
            break; 
        case 10: // enter键
        {
            ITEM *cur = current_item(menu);
            const char *chioce_name = item_name(cur);
            if (strcmp(chioce_name, "退出登录") == 0)
            {
                // 返回主界面
                destroy_menu(menu,items, menu_win);
                next_screen = BACK;
                return next_screen;
            }else if (strcmp(chioce_name, "图书管理") == 0)
            {
                next_screen = BOOK_MANAGE;
                return next_screen;
            }else if (strcmp(chioce_name, "学生管理") == 0)
            {
                next_screen = STUDENT_MANAGE;
                return next_screen;
            }else if (strcmp(chioce_name, "借书记录管理") == 0)
            {
                next_screen = BORROW_RECORDS;
                return next_screen;
            }

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

//图书管理界面
Screen book_manage_info()
{ 
    uiBook**  mybook;
    mybook =  load_books_from_file("text.txt");
    book_list(mybook);
    return BACK;
}

//学生管理界面
Screen stu_manage_info()
{
    uiStudent** mystudent = load_students_from_file("student.txt");
    stu_list(mystudent);  
    return BACK;
}

//借阅管理界面  先不做
Screen borrow_records()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"正在施工");
    wrefresh(win);
    wgetch(win);
    return BACK;
}


//uiStudent *student = &student_set[1];
//学生菜单界面
Screen stu_menu()
{
    char *stu_choices[] = {
        "查看借书记录",
        "借书",
        "还书",
        "退出登录",
    };
    // 清空屏幕
    clear();
    refresh();

    ITEM **items;  
    int n_choices; 

    n_choices = ARRAY_SIZE(stu_choices);
    items = creat_items(stu_choices, n_choices);

    MENU *menu = new_menu((ITEM **)items);
    WINDOW *menu_win = creat_win(terminal.height - 2, terminal.width / 4, 1, 1);
    keypad(menu_win, TRUE);
    

    init_menu_in_win(menu, menu_win,terminal.height - 4, terminal.width / 4 - 2, 1, 1);

    refresh_menu(menu, menu_win);

    WINDOW *info = creat_win(terminal.height - 2, terminal.width - (terminal.width / 4) - 3, 1, terminal.width / 4 + 2);
    wrefresh(info);
    
    Screen next_screen;
    int input_c; // 用户输入的按键,不能为char, 有些按键的宏大于128
    while ((input_c = wgetch(menu_win)) != KEY_F(1))
    {
        switch (input_c)
        {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case KEY_RESIZE:
            clear();
            resize_menu(menu, &menu_win);
            refresh_win(&info);
            break; 
        case 10: // enter键
        {
            ITEM *cur = current_item(menu);
            const char *chioce_name = item_name(cur);
            if (strcmp(chioce_name, "退出登录") == 0)
            {
                // 返回主界面
                destroy_menu(menu,items, menu_win);
                next_screen = BACK;
                return next_screen;
            }
            else if (strcmp(chioce_name, "借书") == 0)
            {
                next_screen = BORROW_BOOK;
                return next_screen;
            }else if (strcmp(chioce_name, "还书") == 0)
            {
                next_screen = RETURN_BOOK;
                return next_screen;
            }else if (strcmp(chioce_name, "查看借书记录") == 0)
            {
               //stu_borrow_list(student_set);
                next_screen = VIEW_RECORDS;
                return next_screen;
            }

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




//借书界面  **
Screen borrow_book()
{
    return BOOK_MANAGE;
}

//还书界面  同下
Screen return_book()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"正在施工");
    wrefresh(win);
    wgetch(win);
    return BACK;
}

// uiBook pbook1 = {21378783974, "9787547008592", "数学之美", "李明", "人民教育出版社", "2020-01-27", 0};
// uiBook pbook2 = {21378783974, "9787547008593", "物理世界", "张伟", "科学出版社", "2020-01-27", 1};
// uiBook pbook3 = {1111111111, "9787547008593", "物理世界", "张伟", "科学出版社", "2020-01-27", 1};
// uiBook *book_set[] = {&pbook1, &pbook2};
// Student student_set[2] = {{213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21, "2021-03-22", "2032-10-21", {&pbook1, &pbook2}},
//                           {213213, "张玮", "计算机21-2", "计算机科学与技术", 21, "2021-03-22", "2032-10-21", {&pbook1, &pbook2, &pbook3}}};



//学生借阅列表  
Screen view_records()
{
    uiStudent ** mystudent = load_books_from_file("student.txt");
    // uiStudent* mystudent = {213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21, "2021-03-22", "2032-10-21", {&pbook1, &pbook2}};
    // stu_borrow_list(*mystudent);
    return BACK;
}













Screen about()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"Oh My Labrasys 是一款专为图书管理设计的软件，界面简洁直观，功能布局清晰。");
    mvwprintw(win,2,1,"还没完呢, 该怎么些, 后面的好难");
    mvwprintw(win,3,1,"12月16号， 今天排版列表花了一下午， 最后还是用了最简单的定位解决了");
    wrefresh(win);
    wgetch(win);
    return BACK;
}


