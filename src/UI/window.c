#include "window.h"
#include "list.h"
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <form.h> 


void show_message_box(const char *message) {
     int height = 5;
    int width = 30;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *msg_win = newwin(height, width, starty, startx);
    box(msg_win, 0, 0);
    mvwprintw(msg_win, 2, 2, "%s", message);
    wrefresh(msg_win);

    // 等待用户按下任意键
    wgetch(msg_win);

    // 删除窗口
    delwin(msg_win);
}


//确认窗口
void show_confirm_window(uiBook* book)
{
    WINDOW* win = newwin(terminal.height / 2, terminal.width / 2, terminal.height / 4, terminal.width / 4);
    box(win, 0, 0);
    keypad(win, TRUE);

    mvwprintw(win, 1, 1, "选择了《%s》, 请输入你你想要的操作!", book->name);
    mvwprintw(win, 3, 1, "ISBN: %s", book->isbn);
    mvwprintw(win, 4, 1, "书名: %s", book->name);
    mvwprintw(win, 5, 1, "作者: %s", book->author);
    mvwprintw(win, 6, 1, "出版社: %s", book->publisher);
    if (book->status == 1)
        mvwprintw(win, 7, 1, "是否可借: 不可借阅");
    else
        mvwprintw(win, 7, 1, "是否可借: 可借阅");

    ITEM** items = (ITEM**)calloc(5, sizeof(ITEM*));
    items[0] = new_item("修改", "");
    items[1] = new_item("删除", "");
    items[2] = new_item("取消", "");
    items[3] = NULL;

    MENU* menu = new_menu((ITEM**)items);
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, 2, 20, 11, 40)); // 调整子窗口尺寸和位置
    set_menu_format(menu, 1, 3);
    post_menu(menu);
    wrefresh(win);


    while (1)
    {   
        Inputinfo* new_info;
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
                ITEM *cur = current_item(menu);
                const char *choice = item_name(cur);
                if(strcmp(choice, "修改") == 0)
                 {
                    new_info = input_info("ISBN", "书名", "作者", "出版社");
                    if (new_info == NULL)
                        return;
                    strcpy(book->isbn, new_info->info1);
                    strcpy(book->name, new_info->info2);
                    strcpy(book->author, new_info->info3);
                    strcpy(book->publisher, new_info->info4);
                    // 需要所有其他信息, 可能要更新
                }
                else if (strcmp(choice, "删除") == 0)
                {
                    show_message_box("已删除!");
                }
                return;// Handle selection
                return;
        }
        wrefresh(win);
    }
}

void show_student_confirm_window(Student* student)
{
    WINDOW* win = newwin(terminal.height / 2, terminal.width / 2, terminal.height / 4, terminal.width / 4);
    box(win, 0, 0);
    keypad(win, TRUE);

    mvwprintw(win, 1, 1, "选择了 %s, 请输入你你想要的操作!", student->name);
    mvwprintw(win, 3, 1, "学号: %s", student->id);
    mvwprintw(win, 4, 1, "姓名: %s", student->name);
    mvwprintw(win, 5, 1, "班级: %s", student->class);
    mvwprintw(win, 6, 1, "学院: %s", student->department);

    ITEM** items = (ITEM**)calloc(5, sizeof(ITEM*));
    items[0] = new_item("修改", "");
    items[1] = new_item("删除", "");
    items[2] = new_item("取消", "");
    items[3] = NULL;

    MENU* menu = new_menu((ITEM**)items);
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, 2, 20, 11, 40)); // 调整子窗口尺寸和位置
    set_menu_format(menu, 1, 3);
    post_menu(menu);
    wrefresh(win);

    while (1)
    {   
        wrefresh(win);
        int ch = wgetch(win);
        Inputinfo* new_info;
        Inputinfo* old_info = calloc(1, sizeof(Inputinfo));
        strcpy(old_info->info1, student->id);
        strcpy(old_info->info2, student->name);
        strcpy(old_info->info3, student->class);
        strcpy(old_info->info4, student->department);

        switch (ch)
        {
            case KEY_LEFT:
                menu_driver(menu, REQ_LEFT_ITEM);
                break;
            case KEY_RIGHT:
                menu_driver(menu, REQ_RIGHT_ITEM);
                break;
            case 10: 
                ITEM *cur = current_item(menu);
                const char *choice = item_name(cur);
                if(strcmp(choice, "修改") == 0)
                {
                    new_info = input_info("学号", "姓名", "班级", "学院");
                    if (new_info == NULL)
                        return;
                    strcpy(student->id, new_info->info1);
                    strcpy(student->name, new_info->info2);
                    strcpy(student->class, new_info->info3);
                    strcpy(student->department, new_info->info4);
                    // 需要所有其他信息, 可能要更新
                }
                else if (strcmp(choice, "删除") == 0)
                {
                    show_message_box("已删除!");
                }
                return;
        }
        wrefresh(win);
    }
}

//用户输入窗口
Inputinfo* input_info(char* a, char* b, char *c, char*d)
{
    Inputinfo *input_info = calloc(1, sizeof(Inputinfo));
    WINDOW* input_wins[4];
    create_windows(input_wins, terminal.height / 8, terminal.width / 2, terminal.height / 4, terminal.width / 4);


    // mvwprintw(input_wins[0], 1, 6, "%s", student->id);
    // mvwprintw(input_wins[1], 1, 6, "%s", student->name);
    // mvwprintw(input_wins[2], 1, 6, "%s", student->class);
    // mvwprintw(input_wins[3], 1, 6, "%s", student->department);



    mvwprintw(input_wins[0], 2, 3, "%s: ",a);
    mvwprintw(input_wins[1], 2, 3, "%s: ",b);
    mvwprintw(input_wins[2], 2, 3, "%s: ",c);
    mvwprintw(input_wins[3], 2, 3, "%s: ",d);

    for(int i = 0; i< 4; i++)
        wrefresh(input_wins[i]);

    char str[50];

    int current_win = 0;
    wmove(input_wins[current_win], 2, 8);
    wrefresh(input_wins[current_win]);

    int ch;
    while ((ch = wgetch(input_wins[current_win])) != 'q') {
        switch (ch) {
            case 27: //esc
                return NULL;
            case 9: // Tab 键
                wmove(input_wins[current_win], 2, 8);
                wrefresh(input_wins[current_win]);
                current_win = (current_win + 1) % 4;
                wmove(input_wins[current_win], 2, 8);
                wrefresh(input_wins[current_win]);
                break;
            case 10: // Enter 键
                destroy_windows(input_wins);
                endwin();
                return input_info;
            default:
                echo();
                waddstr(input_wins[current_win], "                ");
                wmove(input_wins[current_win], 2, 8);
                wrefresh(input_wins[current_win]);
                wgetstr(input_wins[current_win], str);
                switch (current_win)
                {
                    case 0:
                    strcpy(input_info->info1, str);
                    break;
                    case 1:
                    strcpy(input_info->info2, str);
                    break;
                    case 2:
                    strcpy(input_info->info3, str);
                    break;
                    case 3:
                    strcpy(input_info->info4, str);
                    break;
                }
                wmove(input_wins[current_win], 2, 8);
                wrefresh(input_wins[current_win]);
                current_win = (current_win + 1) % 4;
                wmove(input_wins[current_win], 2, 8);
                wrefresh(input_wins[current_win]);
                refresh();
                wrefresh(input_wins[current_win]);
                noecho();
                break;
        }
    }

    
    destroy_windows(input_wins);
    endwin();
    return NULL;

}

void create_windows(WINDOW **wins, int height, int width, int starty, int startx) {
    for (int i = 0; i < 4; i++) {
        wins[i] = newwin(height+1, width, starty + i * (height ), startx);
        if (i > 0)
            wborder(wins[i],'|','|',' ',' ',' ',' ',' ',' ');
        else
            box(wins[i], 0, 0);
        wrefresh(wins[i]);
    }
}

void destroy_windows(WINDOW **wins) {
    for (int i = 0; i < 4; ++i) {
        delwin(wins[i]);
    }
}