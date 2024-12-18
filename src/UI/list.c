#include "UI/utils.h"
#include "UI/list.h"
#include <string.h>

// uiBook book_set[LIST_SIZE] = {
//     {8479248713,"9787547008592", "数学之美", "李明", "人民教育出版社","2022-10-10", 0},
//     {8479248713,"9787547008593", "物理世界", "张伟", "科学出版社","2022-10-10",1},
//     {8479248713,"9787547008594", "化学基础", "王芳", "高等教育出版社","2022-10-10", 0},
//     {8479248713,"9787547008595", "生物学概论", "陈强", "清华大学出版社","2022-10-10", 1},
//     {8479248713,"9787547008596", "地理探索", "赵敏", "北京大学出版社","2022-10-10", 0},
//     {8479248713,"9787547008597", "历史长河", "孙丽", "复旦大学出版社","2022-10-10", 1},
//     {8479248713,"9787547008598", "文学欣赏", "周勇", "上海交通大学出版社","2022-10-10", 1},
//     {8479248713,"9787547008599", "艺术概论", "吴静", "浙江大学出版社", "2022-10-10",1},
//     {8479248713,"9787547008600", "经济学原理", "郑刚", "南京大学出版社", "2022-10-10",1},
//     {8479248713,"9787547008601", "管理学基础", "林霞", "武汉大学出版社","2022-10-10", 1},
//     {8479248713,"9787547008602", "法学导论", "黄磊", "中山大学出版社", "2022-10-10",0},
//     {8479248713,"9787547008603", "教育心理学", "徐娟", "厦门大学出版社", "2022-10-10",0},
//     {8479248713,"9787547008604", "计算机科学", "高明", "哈尔滨工业大学出版社", "2022-10-10",1},
//     {8479248713,"9787547008605", "机械工程", "杨洋", "西安交通大学出版社", "2022-10-10",1},
// };

//图书管理列表
void book_list(uiBook** book_set)
{

    char* menu_choices[] = {
        "添加图书",
        "批量导入",
        "查询图书",
        "退出",
    };

    Infoname infoname = {
        "库存总数",
        "可解本书",
        "管理员",
        "时间",
    };
    
    // 更新终端尺寸
    update_terminal_size();

    // 创建主窗口
    int main_win_height = terminal.height - 9; // 高度减少，在底部留5格
    int main_win_width = terminal.width - 2;
    int main_win_start_y = 4; // 起始位置下降4格
    int main_win_start_x = 1;
    WINDOW* main_win = creat_win(main_win_height,main_win_width, main_win_start_y, main_win_start_x);

    // 创建 info 窗口
    int info_height = 4;
    int info_width = terminal.width - 2; // 宽度减少2
    int info_start_y = 0;
    int info_start_x = 1;
    WINDOW *info_win = creat_win(info_height, info_width, info_start_y, info_start_x);
    mvwprintw(info_win, 0, (terminal.width - 2 - strlen("图书管理界面")) / 2, "图书管理界面");
    //当前书籍信息打印
    MenuInfo info = {150, 100, "admin", "2024-12-17"};
    print_info(info_win,&infoname, &info);
    wrefresh(info_win);

    // 创建 menu 窗口
    int menu_height = 5;
    int menu_width = terminal.width - 2; // 宽度减少2
    int menu_start_y = terminal.height - 5;
    int menu_start_x = 1;
    WINDOW *menu_win = creat_win(menu_height, menu_width,menu_start_y,  menu_start_x);
    keypad(menu_win, TRUE);
    wrefresh(menu_win);

    // 创建菜单项
    int n_choices = sizeof(menu_choices) / sizeof(menu_choices[0]);
    ITEM **items = creat_items(menu_choices, n_choices);

    // 创建菜单
    MENU *menu = new_menu((ITEM **)items);
    set_menu_win(menu, menu_win);

    // 计算菜单项的间隔
    int menu_item_width = (menu_width - 2) / n_choices;
    set_menu_format(menu, 1, n_choices);
    set_menu_spacing(menu, 0, menu_item_width - strlen(menu_choices[0]), 0);


    // 设置菜单子窗口
    set_menu_sub(menu, derwin(menu_win, 1, menu_width - 2, 2, 1));
    set_menu_mark(menu, "  ");
    set_menu_fore(menu, COLOR_PAIR(FONT));
    set_menu_back(menu, COLOR_PAIR(UNHEIGHT));

    // 发布菜单
    post_menu(menu);
    set_current_item(menu, NULL);
    wrefresh(menu_win);

    int scroll_offset = 0;    // 滚动偏移量
    int current_row = 0;      // 当前选中的行
// 创建 pad
    int pad_height = LIST_SIZE + 1;
    int pad_width = terminal.width - 2; // 宽度减少2
    WINDOW *pad = newpad(pad_height, pad_width);
    keypad(pad, TRUE);
REFRESH_BOOK_PAD:
    // 绘制列表项到 pad 上

    for (int i = 0; i < LIST_SIZE; i++) {
        print_list(book_set, pad, i);
    }


    // 高亮当前行
    wattron(pad, A_REVERSE);
    print_list(book_set, pad, current_row);
    wattroff(pad, A_REVERSE);

    // 打印表头
    attron(COLOR_PAIR(PAD_HEIGHT_LIGHT));
    mvwprintw(main_win, 0, 5, "%s", "ISBN");
    mvwprintw(main_win, 0, 25, "%s", "书名");
    mvwprintw(main_win, 0, 40, "%s", "作者");
    mvwprintw(main_win, 0, 60, "%s", "出版社");
    mvwprintw(main_win, 0, 83, "%s", "借阅状态");
    attroff(COLOR_PAIR(PAD_HEIGHT_LIGHT));

    // 刷新 pad 的初始视图
    prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
    wrefresh(main_win);


    // 初始化
    enum { PAD, MENU } active_window = PAD;
    WINDOW *current_win = pad;
    keypad(menu_win, TRUE);

    // 主循环


    while (1) {
        int ch = wgetch(current_win);  
        switch (ch) {
            // 按键处理
            case KEY_UP:
                if (active_window == PAD && current_row > 0) {
                    // 去掉旧的高亮
                    print_list(book_set, pad, current_row);
                    current_row--;
                    if (current_row < scroll_offset && scroll_offset > 0) {
                        scroll_offset--;
                    }
                }
                break;
            case KEY_DOWN:
                if (active_window == PAD && current_row < LIST_SIZE - 1) {
                    // 去掉旧的高亮
                    print_list(book_set, pad, current_row);
                    current_row++;
                    if (current_row >= scroll_offset + main_win_height - 2 && scroll_offset + main_win_height - 2 < pad_height) {
                        scroll_offset++;
                    }
                }
                break;
            case KEY_LEFT:
                if (active_window == MENU) {
                    menu_driver(menu, REQ_LEFT_ITEM);
                }
                break;
            case KEY_RIGHT:
                if (active_window == MENU) {
                    menu_driver(menu, REQ_RIGHT_ITEM);
                }
                break;
            case 9: // Tab 键
                if (active_window == PAD) {
                    active_window = MENU;
                    current_win = menu_win;
                } else {
                    active_window = PAD;
                    current_win = pad;
                }
                // 更新窗口焦点提示
                update_window_focus(menu_win, active_window == MENU);
                update_window_focus(main_win, active_window == PAD);
                break;
            case 10: // 回车键
                if (active_window == MENU) {
                    // 处理菜单选择
                    Inputinfo* new_book;
                    ITEM *cur = current_item(menu);
                    const char *choice = item_name(cur);
                    if (strcmp(choice, "退出") == 0) {
                        
                        // 退出程序
                        unpost_menu(menu);
                        free_menu(menu);
                        for (int i = 0; i < n_choices; ++i) {
                            free_item(items[i]);
                        }
                        free(items);
                        delwin(menu_win);
                        endwin();
                        return;
            
                    }else if (strcmp(choice, "添加图书") == 0)
                    {
                        /*待实现功能, 传入一个结构体, 返回数据, 重新打印到pad
                        * 
                        * uiBook* newBook = getbookinfo();//添加图书
                        * func_addbook(newBook);
                        *
                        */
                       new_book = input_info("ISBN", "书名", "作者", "出版社");
                        if (new_book == NULL)
                            break;
                        show_message_box("已添加");
                    

                        
                    }else if(strcmp(choice, "查询图书") == 0)
                    {
                        /*待实现功能, 传入一个结构体, 返回数据, 重新打印到pad
                        * uiBook* searchBook = getbookinfo();//添加图书
                        * func_findbook(searchBook);
                        *
                        */
                       new_book = my_input_info("ISBN", "书名", "作者", "出版社");
                        if (new_book == NULL)
                            break;
                      
                    }else if(strcmp(choice, "批量导入") == 0)
                    {
                        /*待实现功能, 传入一个结构体, 返回数据, 重新打印到pad
                        * uiBook* searchBook = getbookinfo();//添加图书
                        * func_findbook(searchBook);
                        *
                        */
                        // create_input_window
                    
                    }
                } else if (active_window == PAD) {
                    // 弹出确认窗口
                    show_confirm_window(book_set[current_row]);
                   
                    clear();
                    werase(pad);
                    prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
                    goto REFRESH_BOOK_PAD;
                
                }
                break;
            // 其他按键处理
            case 'q':  // 按 'q' 退出
                unpost_menu(menu);
                free_menu(menu);
                for (int i = 0; i < n_choices; ++i) {
                    free_item(items[i]);
                }
                free(items);
                delwin(menu_win);
                endwin();
                return;
        }

        // 设置新的高亮
        if (active_window == PAD) 
        {
            wattron(pad, A_REVERSE);
            print_list(book_set, pad, current_row);
            wattroff(pad, A_REVERSE);
            set_menu_fore(menu, COLOR_PAIR(UNHEIGHT));
	        set_menu_back(menu, COLOR_PAIR(UNHEIGHT));

        }else
        {
            print_list(book_set, pad, current_row);
            set_menu_fore(menu, COLOR_PAIR(HEIGHTLIGHT) | A_REVERSE);
	        set_menu_back(menu, COLOR_PAIR(FONT));
        }

        // 刷新 pad 的视图
        prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
        // 打印表头
        mvwprintw(main_win, 0, 5, "%s", "ISBN");
        mvwprintw(main_win, 0, 25, "%s", "书名");
        mvwprintw(main_win, 0, 40, "%s", "作者");
        mvwprintw(main_win, 0, 60, "%s", "出版社");
        mvwprintw(main_win, 0, 83, "%s", "借阅状态");
        wrefresh(menu_win);
        wrefresh(main_win);
    }
}

//学生管理列表
void stu_list(uiStudent** student_set)
{
    char* menu_choices[] = {
        "添加学生",
        "批量导入",
        "查询学生",
        "退出",
    };
    // uiBook mybook[] = {{21378783974, "9787547008603", "教育心理学", "徐娟", "厦门大学出版社",  "2020-01-27", 1},
    //     {21378783974, "9787547008604", "计算机科学", "高明", "哈尔滨工业大学出版社",  "2020-01-27", 0},
    //     {21378783974, "9787547008605", "机械工程", "杨洋", "西安交通大学出版社",  "2020-01-27", 0},};
    // uiStudent cur_student = {
    // "54326173883", "张佳玮", "计算机21-2", "计算机科学与技术", 21,"2021-03-22", "2032-10-21",{&mybook[0],&mybook[1],&mybook[2],NULL }};


    
    // uiStudent* student = student_set;
    // 更新终端尺寸
    update_terminal_size();

    // 创建主窗口
    int main_win_height = terminal.height - 9; // 高度减少，在底部留5格
    int main_win_width = terminal.width - 2;
    int main_win_start_y = 4; // 起始位置下降4格
    int main_win_start_x = 1;
    WINDOW* main_win = creat_win(main_win_height,main_win_width, main_win_start_y, main_win_start_x);

    // 创建 info 窗口
    int info_height = 4;
    int info_width = terminal.width - 2; // 宽度减少2
    int info_start_y = 0;
    int info_start_x = 1;
    WINDOW *info_win = creat_win(info_height, info_width, info_start_y, info_start_x);
    mvwprintw(info_win, 0, (terminal.width - 2 - strlen("学生管理界面")) / 2, "学生管理界面");
    //当前学生信息打印
    MenuInfo info = {12, 56, "张三", "2024-12-17"};
    // strncpy(info.name, info.name, sizeof(info.name) - 1);
    // strncpy(info.date, info.date, sizeof(info.date) - 1);
    wrefresh(info_win);

   

    //打印表头
    attron(COLOR_PAIR(PAD_HEIGHT_LIGHT));
    mvwprintw(main_win, 0, 5, "%s", "学号");
    mvwprintw(main_win, 0, 23, "%s", "姓名");
    mvwprintw(main_win, 0, 40, "%s", "班级");
    mvwprintw(main_win, 0, 65, "%s", "学院");
    mvwprintw(main_win, 0, 95, "%s", "借阅数量");
    attroff(COLOR_PAIR(PAD_HEIGHT_LIGHT));

    // 创建 menu 窗口
    int menu_height = 5;
    int menu_width = terminal.width - 2; // 宽度减少2
    int menu_start_y = terminal.height - 5;
    int menu_start_x = 1;
    WINDOW *menu_win = creat_win(menu_height, menu_width,menu_start_y,  menu_start_x);
    keypad(menu_win, TRUE);
    wrefresh(menu_win);

    // 创建菜单项
    int n_choices = sizeof(menu_choices) / sizeof(menu_choices[0]);
    ITEM **items = creat_items(menu_choices, n_choices);

    // 创建菜单
    MENU *menu = new_menu((ITEM **)items);
    set_menu_win(menu, menu_win);

    // 计算菜单项的间隔
    int menu_item_width = (menu_width - 2) / n_choices;
    set_menu_format(menu, 1, n_choices);
    set_menu_spacing(menu, 0, menu_item_width - strlen(menu_choices[0]), 0);

    // 设置菜单子窗口
    set_menu_sub(menu, derwin(menu_win, 1, menu_width - 2, 2, 1));
    set_menu_mark(menu, "  ");
    set_menu_fore(menu, COLOR_PAIR(FONT));
    set_menu_back(menu, COLOR_PAIR(UNHEIGHT));

    // 发布菜单
    post_menu(menu);
    set_current_item(menu, NULL);
    wrefresh(menu_win);

     // 创建 pad
    int pad_height = LIST_SIZE + 1;
    int pad_width = terminal.width - 2; // 宽度减少2

    WINDOW *pad = newpad(pad_height, pad_width);
    keypad(pad, TRUE);
REFRESH_STU_PAD:
    // 绘制列表项到 pad 上
    for (int i = 0; i < LIST_SIZE; i++) {      //长度需要获得
        print_student_list(student_set, pad, i);
    }


    int scroll_offset = 0;    // 滚动偏移量
    int current_row = 0;      // 当前选中的行

    // 高亮当前行
    wattron(pad, A_REVERSE);
    print_student_list(student_set, pad, current_row);
    wattroff(pad, A_REVERSE);

    // 刷新 pad 的初始视图
    prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
    wrefresh(main_win);


    // 初始化
    enum { PAD, MENU } active_window = PAD;
    WINDOW *current_win = pad;
    keypad(menu_win, TRUE);

    // 主循环
    while (1) {
        int ch = wgetch(current_win);
        switch (ch) {
            // 按键处理
            case KEY_UP:
                if (active_window == PAD && current_row > 0) {
                    // 去掉旧的高亮
                    print_student_list(student_set, pad, current_row);
                    current_row--;
                    if (current_row < scroll_offset && scroll_offset > 0) {
                        scroll_offset--;
                    }
                }
                break;
            case KEY_DOWN:
                if (active_window == PAD && current_row < LIST_SIZE - 1) {
                    // 去掉旧的高亮
                    print_student_list(student_set, pad, current_row);
                    current_row++;
                    if (current_row >= scroll_offset + main_win_height - 2 && scroll_offset + main_win_height - 2 < pad_height) {
                        scroll_offset++;
                    }
                }
                break;
            case KEY_LEFT:
                if (active_window == MENU) {
                    menu_driver(menu, REQ_LEFT_ITEM);
                }
                break;
            case KEY_RIGHT:
                if (active_window == MENU) {
                    menu_driver(menu, REQ_RIGHT_ITEM);
                }
                break;
            case 9: // Tab 键
                if (active_window == PAD) {
                    active_window = MENU;
                    current_win = menu_win;
                } else {
                    active_window = PAD;
                    current_win = pad;
                }
                // 更新窗口焦点提示（如果有）
                update_window_focus(menu_win, active_window == MENU);
                update_window_focus(main_win, active_window == PAD);
                break;
            case 10: // 回车键
                if (active_window == MENU) {
                    // 处理菜单选择
                    ITEM *cur = current_item(menu);
                    const char *choice = item_name(cur);
                    Inputinfo* new_student;
                    if (strcmp(choice, "退出") == 0) {
                        // 退出程序
                        unpost_menu(menu);
                        free_menu(menu);
                        for (int i = 0; i < n_choices; ++i) {
                            free_item(items[i]);
                        }
                        free(items);
                        delwin(menu_win);
                        endwin();
                        return;
                    }else if (strcmp(choice, "添加学生") == 0)
                    {
                        new_student = input_info("学号", "姓名", "班级", "学院");
                        if (new_student == NULL)
                            break;
                        show_message_box("已添加");
                    }
                    else if (strcmp(choice, "查询学生") == 0)
                    {
                        new_student = input_info("学号", "姓名", "班级", "学院");
                        if (new_student == NULL)
                            break;
                    }
                    // 其他菜单项的处理逻辑
                } else if (active_window == PAD) {
                    // 弹出确认窗口
                    
                    show_student_confirm_window(student_set[current_row]);
                     werase(pad);
                    goto REFRESH_STU_PAD;
                }
                break;
            // 其他按键处理
            case 'q':  // 按 'q' 退出
                unpost_menu(menu);
                free_menu(menu);
                for (int i = 0; i < n_choices; ++i) {
                    free_item(items[i]);
                }
                free(items);
                delwin(menu_win);
                endwin();
                return;
        }

        // 设置新的高亮
        if (active_window == PAD) 
        {
            wattron(pad, A_REVERSE);
            print_student_list(student_set, pad, current_row);
            wattroff(pad, A_REVERSE);
            set_menu_fore(menu, COLOR_PAIR(UNHEIGHT));
	        set_menu_back(menu, COLOR_PAIR(UNHEIGHT));

        }else
        {
            print_student_list(student_set, pad, current_row);
            set_menu_fore(menu, COLOR_PAIR(HEIGHTLIGHT) | A_REVERSE);
	        set_menu_back(menu, COLOR_PAIR(FONT));
        }

        // 刷新 pad 的视图
        prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
        // 打印表头
        // 打印表头
        attron(COLOR_PAIR(PAD_HEIGHT_LIGHT));
        mvwprintw(main_win, 0, 5, "%s", "学号");
        mvwprintw(main_win, 0, 23, "%s", "姓名");
        mvwprintw(main_win, 0, 40, "%s", "班级");
        mvwprintw(main_win, 0, 65, "%s", "学院");
        mvwprintw(main_win, 0, 95, "%s", "借阅数量");
        attroff(COLOR_PAIR(PAD_HEIGHT_LIGHT));
        wrefresh(menu_win);
        wrefresh(main_win);
    }
}



 
//学生借阅列表
void stu_borrow_list(uiStudent* stu)
{
    Infoname infoname = {
        "姓名",
        "班级",
        "学院",
        "学号",
    };
    //uiStudent* thisStudent = stu[0];
    uiBook** thisBook = stu->books;
    
     char* menu_choices[] = {
        "借书",
        "借阅列表",
        "查询图书",
        "退出",
    };
    
    // 更新终端尺寸
    update_terminal_size();

    // 创建主窗口
    int main_win_height = terminal.height - 9; // 高度减少，在底部留5格
    int main_win_width = terminal.width - 2;
    int main_win_start_y = 4; // 起始位置下降4格
    int main_win_start_x = 1;
    WINDOW* main_win = creat_win(main_win_height,main_win_width, main_win_start_y, main_win_start_x);

    // 创建 info 窗口
    int info_height = 4;
    int info_width = terminal.width - 2; // 宽度减少2
    int info_start_y = 0;
    int info_start_x = 1;
    WINDOW *info_win = creat_win(info_height, info_width, info_start_y, info_start_x);
    mvwprintw(info_win, 0, (terminal.width - 2 - strlen("学生借阅界面")) / 2, "学生借阅界面");
    //当前书籍信息打印
    mvwprintw(info_win, 1, 1, "%s: %s", infoname.field1, stu->name);
    mvwprintw(info_win, 1, 20, "%s: %s", infoname.field2, stu->class);
    mvwprintw(info_win, 1, 40, "%s: %s", infoname.field3, stu->department);
    mvwprintw(info_win, 1, 60, "%s: %ld", infoname.field4, stu->id);
    wrefresh(info_win);


    // 创建 menu 窗口
    int menu_height = 5;
    int menu_width = terminal.width - 2; // 宽度减少2
    int menu_start_y = terminal.height - 5;
    int menu_start_x = 1;
    WINDOW *menu_win = creat_win(menu_height, menu_width,menu_start_y,  menu_start_x);
    keypad(menu_win, TRUE);
    wrefresh(menu_win);

    // 创建菜单项
    int n_choices = sizeof(menu_choices) / sizeof(menu_choices[0]);
    ITEM **items = creat_items(menu_choices, n_choices);

    // 创建菜单
    MENU *menu = new_menu((ITEM **)items);
    set_menu_win(menu, menu_win);

    // 计算菜单项的间隔
    int menu_item_width = (menu_width - 2) / n_choices;
    set_menu_format(menu, 1, n_choices);
    set_menu_spacing(menu, 0, menu_item_width - strlen(menu_choices[0]), 0);


    // 设置菜单子窗口
    set_menu_sub(menu, derwin(menu_win, 1, menu_width - 2, 2, 1));
    set_menu_mark(menu, "  ");
    set_menu_fore(menu, COLOR_PAIR(FONT));
    set_menu_back(menu, COLOR_PAIR(UNHEIGHT));

    // 发布菜单
    post_menu(menu);
    set_current_item(menu, NULL);
    wrefresh(menu_win);

// 创建 pad
    int pad_height = LIST_SIZE + 1;
    int pad_width = terminal.width - 2; // 宽度减少2
    WINDOW *pad = newpad(pad_height, pad_width);
    keypad(pad, TRUE);
REFRESH_BOOK_PAD:
    // 绘制列表项到 pad 上
    for (int i = 0; i < LIST_SIZE; i++) {
        print_list(thisBook, pad, i);
    }

    int scroll_offset = 0;    // 滚动偏移量
    int current_row = 0;      // 当前选中的行

    // 高亮当前行
    wattron(pad, A_REVERSE);
    print_list(thisBook, pad, current_row);
    wattroff(pad, A_REVERSE);

    // 打印表头
    attron(COLOR_PAIR(PAD_HEIGHT_LIGHT));
    mvwprintw(main_win, 0, 5, "%s", "ISBN");
    mvwprintw(main_win, 0, 25, "%s", "书名");
    mvwprintw(main_win, 0, 40, "%s", "作者");
    mvwprintw(main_win, 0, 60, "%s", "出版社");
    mvwprintw(main_win, 0, 83, "%s", "借阅状态");
    attroff(COLOR_PAIR(PAD_HEIGHT_LIGHT));

    // 刷新 pad 的初始视图
    prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
    wrefresh(main_win);


    // 初始化
    enum { PAD, MENU } active_window = PAD;
    WINDOW *current_win = pad;
    keypad(menu_win, TRUE);

    // 主循环

    while (1) {
        int ch = wgetch(current_win);  
        switch (ch) {
            // 按键处理
            case KEY_UP:
                if (active_window == PAD && current_row > 0) {
                    // 去掉旧的高亮
                    print_list(thisBook, pad, current_row);
                    current_row--;
                    if (current_row < scroll_offset && scroll_offset > 0) {
                        scroll_offset--;
                    }
                }
                break;
            case KEY_DOWN:
                if (active_window == PAD && current_row < LIST_SIZE - 1) {
                    // 去掉旧的高亮
                    print_list(thisBook, pad, current_row);
                    current_row++;
                    if (current_row >= scroll_offset + main_win_height - 2 && scroll_offset + main_win_height - 2 < pad_height) {
                        scroll_offset++;
                    }
                }
                break;
            case KEY_LEFT:
                if (active_window == MENU) {
                    menu_driver(menu, REQ_LEFT_ITEM);
                }
                break;
            case KEY_RIGHT:
                if (active_window == MENU) {
                    menu_driver(menu, REQ_RIGHT_ITEM);
                }
                break;
            case 9: // Tab 键
                if (active_window == PAD) {
                    active_window = MENU;
                    current_win = menu_win;
                } else {
                    active_window = PAD;
                    current_win = pad;
                }
                // 更新窗口焦点提示
                update_window_focus(menu_win, active_window == MENU);
                update_window_focus(main_win, active_window == PAD);
                break;
            case 10: // 回车键
                if (active_window == MENU) {
                    // 处理菜单选择
                    Inputinfo* new_book;
                    ITEM *cur = current_item(menu);
                    const char *choice = item_name(cur);
                    if (strcmp(choice, "退出") == 0) {
                        
                        // 退出程序
                        unpost_menu(menu);
                        free_menu(menu);
                        for (int i = 0; i < n_choices; ++i) {
                            free_item(items[i]);
                        }
                        free(items);
                        delwin(menu_win);
                        endwin();
                        return;
            
                    }else if (strcmp(choice, "借书") == 0)
                    {
                        /*待实现功能, 传入一个结构体, 返回数据, 重新打印到pad
                        * 
                        * uiBook* newBook = getbookinfo();//添加图书
                        * func_addbook(newBook);
                        *
                        */
                       new_book = input_info("ISBN", "书名", "作者", "出版社");
                        if (new_book == NULL)
                            break;
                        show_message_box("已借阅");
                    

                        
                    }else if(strcmp(choice, "借阅列表") == 0)
                    {
                        /*待实现功能, 传入一个结构体, 返回数据, 重新打印到pad
                        * uiBook* searchBook = getbookinfo();//添加图书
                        * func_findbook(searchBook);
                        *
                        */

                       new_book = input_info("ISBN", "书名", "作者", "出版社");
                        if (new_book == NULL)
                            break;
                    }else if (strcmp(choice, "查询图书") == 0)
                    {
                        new_book = input_info("ISBN", "书名", "作者", "出版社");
                        if (new_book == NULL)
                            break;
                    }
                } else if (active_window == PAD) {
                    // 弹出确认窗口
                    show_confirm_window(thisBook[current_row]);
                    werase(pad);
                    goto REFRESH_BOOK_PAD;
                
                }
                break;
            // 其他按键处理
            case 'q':  // 按 'q' 退出
                unpost_menu(menu);
                free_menu(menu);
                for (int i = 0; i < n_choices; ++i) {
                    free_item(items[i]);
                }
                free(items);
                delwin(menu_win);
                endwin();
                return;
        }

        // 设置新的高亮
        if (active_window == PAD) 
        {
            wattron(pad, A_REVERSE);
            print_list(thisBook, pad, current_row);
            wattroff(pad, A_REVERSE);
            set_menu_fore(menu, COLOR_PAIR(UNHEIGHT));
	        set_menu_back(menu, COLOR_PAIR(UNHEIGHT));

        }else
        {
            print_list(thisBook, pad, current_row);
            set_menu_fore(menu, COLOR_PAIR(HEIGHTLIGHT) | A_REVERSE);
	        set_menu_back(menu, COLOR_PAIR(FONT));
        }

        // 刷新 pad 的视图
        prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
        // 打印表头
        mvwprintw(main_win, 0, 5, "%s", "ISBN");
        mvwprintw(main_win, 0, 25, "%s", "书名");
        mvwprintw(main_win, 0, 40, "%s", "作者");
        mvwprintw(main_win, 0, 60, "%s", "出版社");
        mvwprintw(main_win, 0, 83, "%s", "借阅状态");
        wrefresh(menu_win);
        wrefresh(main_win);
    }
    
}