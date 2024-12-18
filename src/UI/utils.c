#include "utils.h"
#include "list.h"


void init_all()
{
    // 设置本地化以支持多字节字符
    setlocale(LC_ALL, "");
    // 初始设置curses
    initscr();
    start_color();
    init_pair(HEIGHTLIGHT, COLOR_CYAN, COLOR_WHITE);//第一个为高亮色
    init_pair(UNHEIGHT, COLOR_WHITE, COLOR_BLACK);//第一个为高亮色
    init_pair(FONT, COLOR_WHITE, COLOR_BLACK);//第一个为字体色
    init_pair(PAD_HEIGHT_LIGHT, COLOR_BLUE, COLOR_CYAN);
    init_pair(FOCUSED, COLOR_YELLOW, COLOR_BLACK); // 定义颜色对


    cbreak();
    noecho();
    keypad(stdscr, true);
    update_terminal_size();
    // if (terminal.width < 87 || terminal.height < 24)
    // {
    //     endwin();
    //     fprintf(stderr, "终端尺寸过小，请调整终端窗口大小。\n");
    //     exit(1);
    // }
}

//计算终端的大小, 用于动态布局
TerminalSize terminal;
void update_terminal_size() {
    getmaxyx(stdscr, terminal.height, terminal.width);
}


// 创建菜单项
// 参数:
//   choices: 菜单项的字符串数组
//   n_choices: 菜单项的数量
// 返回值:
//   ITEM**: 指向ITEM指针数组的指针，表示创建的菜单项
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

// 创建一个新的窗口
// 参数:
//   hight: 窗口的高度
//   width: 窗口的宽度
//   y: 窗口左上角的y坐标
//   x: 窗口左上角的x坐标
// 返回值:
//   WINDOW*: 指向新创建的窗口的指针
WINDOW* creat_win(int hight, int width, int y, int x)
{
    WINDOW* win;
    win = newwin(hight, width, y, x);
    box(win, 0, 0);
    wrefresh(win);
    return win;
}

// 初始化菜单并将其放置在窗口中
// 参数:
//   menu: 指向MENU结构体的指针
//   win: 指向WINDOW结构体的指针
//   hight: 菜单子窗口的高度
//   width: 菜单子窗口的宽度
//   y: 菜单子窗口左上角的y坐标
//   x: 菜单子窗口左上角的x坐标
void init_menu_in_win(MENU* menu, WINDOW* win, int hight, int width, int y, int x)
{
    // init_pair(HEIGHTLIGHT, COLOR_CYAN, COLOR_WHITE);//第一个为高亮色
    // init_pair(FONT, COLOR_WHITE, COLOR_BLACK);//第一个为字体色
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, hight, width, y, x));
    keypad(win, true);
    set_menu_mark(menu, "*");
    set_menu_fore(menu, COLOR_PAIR(HEIGHTLIGHT) | A_REVERSE);
	set_menu_back(menu, COLOR_PAIR(FONT));
    set_menu_spacing(menu, 0, 2, 0);
}

// 发布菜单和窗口
// 参数:
//   menu: 指向MENU结构体的指针
//   win: 指向WINDOW结构体的指针
void refresh_menu(MENU* menu, WINDOW* win)
{
    post_menu(menu);
    wrefresh(win);
}

// 调整主菜单的大小
// 参数:
//   menu: 指向MENU结构体的指针
//   win: 指向WINDOW指针的指针
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

// 销毁菜单及其相关资源
// 参数:
//   menu: 指向MENU结构体的指针
//   items: 指向ITEM指针数组的指针
//   menu_win: 指向WINDOW结构体的指针
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

// 调整菜单大小
// 参数:
//   menu: 指向MENU结构体的指针
//   win: 指向WINDOW指针的指针
void resize_menu(MENU *menu, WINDOW **win)
{
    unpost_menu(menu);  // 取消发布菜单
    refresh_win(win);
    post_menu(menu);
    wrefresh(menu_sub(menu));  // 刷新子窗口
    wrefresh(*win);
}

// 刷新窗口
// 参数:
//   win: 指向WINDOW指针的指针
void refresh_win(WINDOW** win)
{
    if (*win != NULL) {
        int height, width, y, x;
        getmaxyx(*win, height, width);
        getbegyx(*win, y, x);
        delwin(*win);
        
        *win = creat_win(height, width, y, x);
        keypad(*win, TRUE);
        box(*win, 0, 0);
        wrefresh(*win);
    }
}


//打印pad的每一项,到缓冲区
void print_list(uiBook* books, WINDOW* pad, int i)
{ 
    mvwprintw(pad, i, 0, "%s", books[i].isbn);
    mvwprintw(pad, i, 22, "%s", books[i].name); 
    mvwprintw(pad, i, 39, "%s", books[i].author); 
    mvwprintw(pad, i, 56, "%s", books[i].publisher);
    if(books[i].status == 1) 
        mvwprintw(pad, i, 82, "可借阅"); 
    else         
        mvwprintw(pad, i, 82, "不可借阅"); 

}

//打印学生pad的每一项,到缓冲区
void print_student_list(Student* students, WINDOW* pad, int i)
{ 
    mvwprintw(pad, i, 0, "%s", students[i].id);
    mvwprintw(pad, i, 20, "%s", students[i].name); 
    mvwprintw(pad, i, 40, "%s", students[i].class); 
    mvwprintw(pad, i, 60, "%s", students[i].department); 
    mvwprintw(pad, i, 80, "%d", students[i].Borrowed_Count); 
}

//提示高亮选中框
void update_window_focus(WINDOW *win, bool focused) 
{
    if (focused) {
        wattron(win, COLOR_PAIR(FOCUSED));
    } else {
        wattroff(win, COLOR_PAIR(FOCUSED));
    }
    box(win, 0, 0);
    wrefresh(win);
}

//打印顶部信息框
void print_info(WINDOW* info_win, Infoname* infoname, MenuInfo* info)
{
    //int width = getmaxx(info_win);
    //mvwprintw(info_win, 0, (width - strlen(infoname->field1)) / 2, "图书管理界面");
    mvwprintw(info_win, 1, 1, "%s: %zu", infoname->field1,info->all);
    mvwprintw(info_win, 1, 20, "%s: %zu",infoname->field2, info->acceess);
    mvwprintw(info_win, 1, 40, "%s: %s", infoname->field3,info->name);
    mvwprintw(info_win, 1, 60, "%s: %s", infoname->field4,info->date);
    wrefresh(info_win);
}
