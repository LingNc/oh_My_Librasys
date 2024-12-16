#include "utils.h"
#include "list.h"
#include "terminal.h"
#include <string.h>

void print_list(Book* books, WINDOW* pad, int i)
{ 
    mvwprintw(pad, i, 0, "%s", books[i].isbn);
    mvwprintw(pad, i, 30, "%s", books[i].name); // 调整起始位置
    mvwprintw(pad, i, 60, "%s", books[i].author); // 调整起始位置
    mvwprintw(pad, i, 90, "%s", books[i].publisher); // 调整起始位置
    mvwprintw(pad, i, 120, "%d", books[i].num); // 调整起始位置
}
void update_window_focus(WINDOW *win, bool focused) {
    if (focused) {
        wattron(win, COLOR_PAIR(4));
    } else {
        wattroff(win, COLOR_PAIR(4));
    }
    box(win, 0, 0);
    wrefresh(win);
}

void print_info(WINDOW* info_win, Info* info)
{
    int width = getmaxx(info_win);
    mvwprintw(info_win, 0, (width - strlen("图书管理界面")) / 2, "图书管理界面");
    mvwprintw(info_win, 1, 1, "总数: %zu", info->all);
    mvwprintw(info_win, 1, 20, "可借: %zu", info->acceess);
    mvwprintw(info_win, 1, 40, "管理员: %s", info->name);
    mvwprintw(info_win, 1, 60, "时间: %s", info->date);
    wrefresh(info_win);
}


void show_confirm_window(Book* book)
{
    WINDOW* win = newwin(terminal.height / 2, terminal.width / 2, terminal.height / 4, terminal.width / 4);
    box(win, 0, 0);
    keypad(win, TRUE);

    mvwprintw(win, 1, 1, "选择了《%s》, 请输入你你想要的操作!", book->name);

    ITEM** items = (ITEM**)calloc(5, sizeof(ITEM*));
    items[0] = new_item("详情", "");
    items[1] = new_item("修改", "");
    items[2] = new_item("删除", "");
    items[3] = new_item("取消", "");
    items[4] = NULL;

    MENU* menu = new_menu((ITEM**)items);
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, 4, 40, 3, 1)); // 调整子窗口尺寸和位置
    set_menu_format(menu, 1, 4);
    post_menu(menu);
    wrefresh(win);

    while (1)
    {   
        int ch = wgetch(win);
        switch (ch)
        {
            case KEY_LEFT:
                menu_driver(menu, REQ_LEFT_ITEM);
                break;
            case KEY_RIGHT:
                menu_driver(menu, REQ_RIGHT_ITEM);
                break;
            case 10: // Enter key
                // Handle selection
                return;
        }
        wrefresh(win);
    }
}

void list()
{
    char* menu_choices[] = {
        "添加图书",
        "批量导入",
        "查询图书",
        "退出",
    };
    Book books[LIST_SIZE] = {
        {"9787547008592", "数学之美", "李明", "人民教育出版社", 5},
        {"9787547008593", "物理世界", "张伟", "科学出版社", 4},
        {"9787547008594", "化学基础", "王芳", "高等教育出版社", 6},
        {"9787547008595", "生物学概论","陈强", "清华大学出版社", 7},
        {"9787547008596", "地理探索", "赵敏", "北京大学出版社", 8},
        {"9787547008597", "历史长河", "孙丽", "复旦大学出版社", 9},
        {"9787547008598", "文学欣赏", "周勇", "上海交通大学出版社", 10},
        {"9787547008599", "艺术概论", "吴静", "浙江大学出版社", 11},
        {"9787547008600", "经济学原理", "郑刚", "南京大学出版社", 12},
        {"9787547008601", "管理学基础", "林霞", "武汉大学出版社", 13},
        {"9787547008602", "法学导论", "黄磊", "中山大学出版社", 14},
        {"9787547008603", "教育心理学", "徐娟", "厦门大学出版社", 15},
        {"9787547008604", "计算机科学", "高明", "哈尔滨工业大学出版社", 16},
        {"9787547008605", "机械工程", "杨洋", "西安交通大学出版社", 17},
      };

    // 更新终端尺寸
    update_terminal_size();
init_pair(4, COLOR_YELLOW, COLOR_BLACK); // 定义颜色对

    // 创建主窗口
    int main_win_height = terminal.height - 9; // 高度减少，在底部留5格
    int main_win_width = terminal.width - 2;
    int main_win_start_y = 4; // 起始位置下降4格
    int main_win_start_x = 1;
    WINDOW *main_win = newwin(main_win_height, main_win_width, main_win_start_y, main_win_start_x);
    box(main_win, 0, 0);
    wrefresh(main_win);

    // 创建 info 窗口
    int info_height = 4;
    int info_width = terminal.width - 2; // 宽度减少2
    int info_start_y = 0;
    int info_start_x = 1;
    WINDOW *info_win = newwin(info_height, info_width, info_start_y, info_start_x);
    box(info_win, 0, 0);
    mvwprintw(info_win, 1, (info_width - strlen("图书管理界面")) / 2, "图书管理界面");
    wrefresh(info_win);

    // 创建 pad
    int pad_height = LIST_SIZE + 1;
    int pad_width = terminal.width - 2; // 宽度减少2
    WINDOW *pad = newpad(pad_height, pad_width);
    keypad(pad, TRUE);

    // 绘制列表项到 pad 上
    for (int i = 0; i < LIST_SIZE; i++) {
        print_list(books, pad, i);
    }

    int scroll_offset = 0;    // 滚动偏移量
    int current_row = 0;      // 当前选中的行

    // 高亮当前行
    wattron(pad, A_REVERSE);
    print_list(books, pad, current_row);
    wattroff(pad, A_REVERSE);

    init_pair(3, COLOR_BLUE, COLOR_CYAN);
    attron(COLOR_PAIR(3));
    mvwprintw(main_win, 0, 5, "%s", "ISBN");
    mvwprintw(main_win, 0, 23, "%s", "书名");
    mvwprintw(main_win, 0, 40, "%s", "作者");
    mvwprintw(main_win, 0, 55, "%s", "出版社");
    mvwprintw(main_win, 0, 90, "%s", "数量");
    attroff(COLOR_PAIR(3));

    // 创建 menu 窗口
    int menu_height = 5;
    int menu_width = terminal.width - 2; // 宽度减少2
    int menu_start_y = terminal.height - 5;
    int menu_start_x = 1;
    WINDOW *menu_win = newwin(menu_height, menu_width, menu_start_y, menu_start_x);
    box(menu_win, 0, 0);
    keypad(menu_win, TRUE);
    wrefresh(menu_win);

    // 创建菜单项
    int n_choices = sizeof(menu_choices) / sizeof(menu_choices[0]);
    ITEM **items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_choices; ++i) {
        items[i] = new_item(menu_choices[i], "");
    }
    items[n_choices] = NULL;

    // 创建菜单
    MENU *menu = new_menu((ITEM **)items);
    set_menu_win(menu, menu_win);

    // 计算菜单项的间隔
    int menu_item_width = (menu_width - 2) / n_choices;
    set_menu_format(menu, 1, n_choices);
    set_menu_spacing(menu, 0, menu_item_width - strlen(menu_choices[0]), 0);

    // 设置菜单子窗口
    set_menu_sub(menu, derwin(menu_win, 1, menu_width - 2, 2, 1));
    set_menu_mark(menu, " * ");

    // 发布菜单
    post_menu(menu);
    wrefresh(menu_win);

    // 刷新 pad 的初始视图
    prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
    wrefresh(main_win);

    Info info = {150, 100, "admin", "2024-12-17"};
    print_info(info_win, &info);

    // 初始化
    enum { PAD, MENU } active_window = PAD;
    WINDOW *current_win = pad;
    //keypad(menu_win, TRUE);

    // 主循环
    while (1) {
        int ch = wgetch(current_win);
        switch (ch) {
            // 按键处理
            case KEY_UP:
                if (active_window == PAD && current_row > 0) {
                    // 去掉旧的高亮
                    print_list(books, pad, current_row);
                    current_row--;
                    if (current_row < scroll_offset && scroll_offset > 0) {
                        scroll_offset--;
                    }
                }
                break;
            case KEY_DOWN:
                if (active_window == PAD && current_row < LIST_SIZE - 1) {
                    // 去掉旧的高亮
                    print_list(books, pad, current_row);
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
                    }
                    // 其他菜单项的处理逻辑
                } else if (active_window == PAD) {
                    // 弹出确认窗口
                    show_confirm_window(&books[current_row]);
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
        if (active_window == PAD) {
            wattron(pad, A_REVERSE);
            print_list(books, pad, current_row);
            wattroff(pad, A_REVERSE);
        }

        // 刷新 pad 的视图
        prefresh(pad, scroll_offset, 0, main_win_start_y + 1, main_win_start_x + 1, main_win_start_y + main_win_height - 2, main_win_start_x + main_win_width - 2);
    }
}
