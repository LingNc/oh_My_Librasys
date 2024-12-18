#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "UI/utils.h"
#include "UI/list.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))    //只能在主函数用,指针退化,无法得出

uiBook books[20];
uiStudent students[20];

void display_title()
{
    update_terminal_size();
    int width = (terminal.width - 87) / 2;
    mvprintw(4, width, "                __         __  ___         __    _ __               _____              ");
    mvprintw(5, width, "         ____  / /_       /  |/  /_  __   / /   (_) /_  _________ _/ ___/__  _______   ");
    mvprintw(6, width, "        / __ \\/ __ \\     / /|_/ / / / /  / /   / / __ \\/ ___/ __ `/\\__ \\/ / / / ___/   ");
    mvprintw(7, width, "       / /_/ / / / /    / /  / / /_/ /  / /___/ / /_/ / /  / /_/ /___/ / /_/ (__  )    ");
    mvprintw(8, width, "       \\____/_/ /_/    /_/  /_/\\__, /  /_____/_/_.___/_/   \\__,_//____/\\__, /____/     ");
    mvprintw(9, width, "                              /____/                                  /____/           ");
    mvprintw(LINES - 3, 0, "按 <ENTER> 选择");
    mvprintw(LINES - 2, 0, "上下选择, 退格键退出");
    refresh();
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

// uiBook books[LIST_SIZE] = {
    //     {21378783974, "9787547008592", "数学之美", "李明", "人民教育出版社", "2020-01-27", 0},
    //     {21378783974, "9787547008593", "物理世界", "张伟", "科学出版社",  "2020-01-27", 1},
    //     {21378783974, "9787547008594", "化学基础", "王芳", "高等教育出版社",  "2020-01-27", 1},
    //     {21378783974, "9787547008595", "生物学概论","陈强", "清华大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008596", "地理探索", "赵敏", "北京大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008597", "历史长河", "孙丽", "复旦大学出版社",  "2020-01-27", 1},
    //     {21378783974, "9787547008598", "文学欣赏", "周勇", "上海交通大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008599", "艺术概论", "吴静", "浙江大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008600", "经济学原理", "郑刚", "南京大学出版社",  "2020-01-27", 1},
    //     {21378783974, "9787547008601", "管理学基础", "林霞", "武汉大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008602", "法学导论", "黄磊", "中山大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008603", "教育心理学", "徐娟", "厦门大学出版社",  "2020-01-27", 1},
    //     {21378783974, "9787547008604", "计算机科学", "高明", "哈尔滨工业大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008605", "机械工程", "杨洋", "西安交通大学出版社",  "2020-01-27", 0},
    //     {1, "978-3-16-148410-0", "Book1", "Author1", "Publisher1", "2023-01-01", 1},
    // {2, "978-0-13-149509-9", "Book2", "Author2", "Publisher2", "2022-05-15", 1},
    // {3, "978-1-84951-177-3", "Book3", "Author3", "Publisher3", "2021-10-30", 0},
    // {4, "978-0-7486-2585-7", "Book4", "Author4", "Publisher4", "2020-04-12", 1},
    // {5, "978-1-4088-8543-8", "Book5", "Author5", "Publisher5", "2019-11-22", 1},
    // {6, "978-0-596-52068-7", "Book6", "Author6", "Publisher6", "2018-08-05", 0},
    // {7, "978-1-4493-2586-2", "Book7", "Author7", "Publisher7", "2017-03-18", 0},
    // {8, "978-0-201-63361-2", "Book8", "Author8", "Publisher8", "2016-12-10", 1},
    // {9, "978-1-4493-6537-3", "Book9", "Author9", "Publisher9", "2015-07-25", 0},
    // {10, "978-0-13-235088-4", "Book10", "Author10", "Publisher10", "2014-02-14", 1}
    //   };

Screen book_manage_info()
{ 

    book_list(books);
    return BACK;
}

uiBook book1 = {8479248713,"9787547008603", "教育心理学", "徐娟", "厦门大学出版社", "2022-10-10",0};
uiBook book2 = {8479248713,"9787547008604", "计算机科学", "高明", "哈尔滨工业大学出版社", "2022-10-10",1};
uiBook book3 = {8479248713,"9787547008600", "经济学原理", "郑刚", "南京大学出版社", "2022-10-10",1};

uiStudent student_set[LIST_SIZE] = {
    {213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{NULL}},
    {213213, "张玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{&book1, &book2, &book3}},
    {213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{NULL}},
    {213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{NULL}},
    {213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{NULL}},
    {213213, "张佳玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{NULL}},
};
Screen stu_manage_info()
{
    
    stu_list(student_set);  
    return BACK;
}

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
                stu_borrow_list(students);
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

Screen search_books()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"正在施工");
    wrefresh(win);
    wgetch(win);
    return BACK;
}

Screen borrow_book()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"正在施工");
    wrefresh(win);
    wgetch(win);
    return BACK;
}

Screen return_book()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"正在施工");
    wrefresh(win);
    wgetch(win);
    return BACK;
}

Screen view_records()
{
    WINDOW* win = newwin(terminal.height/2,terminal.width/2, terminal.height/4,terminal.width/4);
    box(win,0,0);
    mvwprintw(win,1,1,"正在施工");
    wrefresh(win);
    wgetch(win);
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


