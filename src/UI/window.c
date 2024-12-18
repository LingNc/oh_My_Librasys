#include "UI/window.h"
#include "UI/list.h"
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
    mvwprintw(win, 3, 1, "ISBN: %s", book->ISBN);
    mvwprintw(win, 4, 1, "书名: %s", book->name);
    mvwprintw(win, 5, 1, "作者: %s", book->author);
    mvwprintw(win, 6, 1, "出版社: %s", book->publisher);
    if (book->status == 1)
        mvwprintw(win, 7, 1, "是否可借: 不可借阅");
    else if(book->status == 0)
        mvwprintw(win, 7, 1, "是否可借: 可借阅");
    else    //删除
        mvwprintw(win, 7, 1, "");


    ITEM** items = (ITEM**)calloc(5, sizeof(ITEM*));
    items[0] = new_item("修改", "");
    items[1] = new_item("删除", "");
    items[2] = new_item("取消", "");
    items[3] = NULL;

    MENU* menu = new_menu((ITEM**)items);
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, 2, 20, 15, 40)); // 调整子窗口尺寸和位置
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
                    // new_info = my_input_info("ISBN", "书名", "作者", "出版社");
                    // if (new_info == NULL)
                    //     return;

                    char *ISBN = simplewin("请输入ISBN号: ");
                    char *bookname = simplewin("请输入书名: ");
                    char *author = simplewin("请输入作者: ");
                    char *publisher = simplewin("请输入出版社: ");

                
                    strcpy(book->ISBN, ISBN);
                    strcpy(book->name, bookname);
                    strcpy(book->author, author);
                    strcpy(book->publisher, publisher);
                    book->status = -1;
                    // 需要所有其他信息, 可能要更新
                    // wrefresh(win);
                    return ;
                }
                else if (strcmp(choice, "删除") == 0)
                {
                    strcpy(book->ISBN, "");
                    strcpy(book->name, "");
                    strcpy(book->author, "");
                    strcpy(book->publisher, "");
                    book->status = -1;
                    // 需要所有其他信息, 可能要更新
                    size_t lea_book_id = book->id;
                    wrefresh(win);
                    return ;
                    show_message_box("已删除!");
                }
                return;// Handle selection
                return;
        }
        wrefresh(win);
    }
}




Inputinfo* input_info(char* a, char* b, char* c, char* d)
{
    setlocale(LC_ALL, "");

    Inputinfo *input_info = calloc(1, sizeof(Inputinfo));
    if (!input_info)
        return NULL;

    WINDOW* input_wins[4];
    create_windows(input_wins, terminal.height / 8, terminal.width / 2, terminal.height / 4, terminal.width / 4);

    char* labels[4] = {a, b, c, d};
    char input_buffers[4][50] = { "", "", "", "" };

    for(int i = 0; i < 4; i++) {
        mvwprintw(input_wins[i], 2, 3, "%s: %s", labels[i], input_buffers[i]);
        wmove(input_wins[i], 2, 8 + strlen(input_buffers[i]));
        wrefresh(input_wins[i]);
        keypad(input_wins[i], TRUE);
    }

    int current_win = 0;
    wrefresh(input_wins[current_win]);

    int ch;
    while (1) {
        wget_wch(input_wins[current_win], &ch);
        switch (ch) {
            case 27: // Esc
                free(input_info);
                destroy_windows(input_wins);
                return NULL;

            case KEY_UP:
                current_win = (current_win - 1 + 4) % 4;
                wmove(input_wins[current_win], 2, 8 + strlen(input_buffers[current_win]));
                wrefresh(input_wins[current_win]);
                break;

            case KEY_DOWN:
                current_win = (current_win + 1) % 4;
                wmove(input_wins[current_win], 2, 8 + strlen(input_buffers[current_win]));
                wrefresh(input_wins[current_win]);
                break;

            case 10: // Enter
                if (current_win < 3) {
                    current_win = (current_win + 1) % 4;
                    wmove(input_wins[current_win], 2, 8 + strlen(input_buffers[current_win]));
                    wrefresh(input_wins[current_win]);
                } else {
                    strcpy(input_info->info1, input_buffers[0]);
                    strcpy(input_info->info2, input_buffers[1]);
                    strcpy(input_info->info3, input_buffers[2]);
                    strcpy(input_info->info4, input_buffers[3]);
                    destroy_windows(input_wins);
                    return input_info;
                }
                break;

            case KEY_BACKSPACE:
            case 127: // Backspace
                if (strlen(input_buffers[current_win]) > 0) {
                    input_buffers[current_win][strlen(input_buffers[current_win]) - 1] = '\0';
                    werase(input_wins[current_win]);
                    mvwprintw(input_wins[current_win], 2, 3, "%s: %s", labels[current_win], input_buffers[current_win]);
                    wmove(input_wins[current_win], 2, 8 + strlen(input_buffers[current_win]));
                    wrefresh(input_wins[current_win]);
                }
                break;

            default:
                if (iswprint(ch) && strlen(input_buffers[current_win]) < 49) {
                    char utf8_char[4];
                    int state;
                    memset(&state, 0, sizeof(state));
                    wcrtomb(utf8_char, ch, &state);
                    strncat(input_buffers[current_win], utf8_char, sizeof(input_buffers[current_win]) - strlen(input_buffers[current_win]) - 1);
                    wadd_wch(input_wins[current_win], &ch);
                    wrefresh(input_wins[current_win]);
                }
                break;
        }
    }

    destroy_windows(input_wins);
    free(input_info);
    return NULL;
}









void show_student_confirm_window(uiStudent* student)
{
    WINDOW* win = newwin(terminal.height / 2, terminal.width / 2, terminal.height / 4, terminal.width / 4);
    box(win, 0, 0);
    keypad(win, TRUE);

    mvwprintw(win, 1, 1, "选择了 %s, 请输入你你想要的操作!", student->name);
    mvwprintw(win, 3, 1, "学号: %ld", student->id);
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
        char str[20];
        sprintf(str, "%ld", student->id);
        strcpy(old_info->info1, str);
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
                    new_info = my_input_info("学号", "姓名", "班级", "学院");
                    if (new_info == NULL)
                        return;
                        char str[20];
                        int id = atoi(new_info->info1);
                    student->id = id;
                    strcpy(student->name, new_info->info2);
                    strcpy(student->class, new_info->info3);
                    strcpy(student->department, new_info->info4);
                    // 需要所有其他信息, 可能要更新
                    return;
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
Inputinfo*  my_input_info(char* a, char* b, char *c, char*d)
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



void  get_file_address() {

    int ch;
    int start_y;
    FORM *form;
    FIELD *fields[2];
    WINDOW *win_body, *win_form;
    win_body = newwin(24, 80, 0, 0);
    assert(win_body != NULL);
    box(win_body,0,0);
    wrefresh(win_body);

    // Calculate starting line to center the form vertically
    start_y = (LINES - 4) / 2;

    // Create the form window, centered vertically
    win_form = newwin(4, 80, start_y, 0);
    assert(win_form != NULL);
    box(win_form, 0, 0);

    // Display the prompt
    mvwprintw(win_form, 1, 2, "请输入文件地址: ");

    // Display "批量导入" at the upper right corner
    mvwprintw(win_form, 0, 72, "批量导入");

    wrefresh(win_form);

    // Create the input field
    fields[0] = new_field(1, 75, 1, 15, 0, 0); // Adjusted width and position
    fields[1] = NULL;
    assert(fields[0] != NULL);

    set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_back(fields[0], A_UNDERLINE);

    // Create and post the form
    form = new_form(fields);
    assert(form != NULL);
    set_form_win(form, win_form);
    set_form_sub(form, derwin(win_form, 2, 75, 1, 15)); // Adjusted subwindow
    post_form(form);

    wrefresh(win_form);

    // Main input loop
    while ((ch = getch()) != '\n')
    {
        switch (ch)
        {
            case KEY_LEFT:
                form_driver(form, REQ_PREV_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(form, REQ_NEXT_CHAR);
                break;
            case KEY_BACKSPACE:
            case 127:
                form_driver(form, REQ_DEL_PREV);
                break;
            case KEY_DC:
                form_driver(form, REQ_DEL_CHAR);
                break;
            default:
                form_driver(form, ch);
                break;
        }
        wrefresh(win_form);
    }

    // Cleanup
    unpost_form(form);
    free_form(form);
    free_field(fields[0]);
    delwin(win_form);
    delwin(win_body);
    endwin();
}





char* simplewin(char* question)
{
    WINDOW* win = newwin(terminal.height / 2, terminal.width / 2, terminal.height / 4, terminal.width / 4);
    box(win, 0, 0);
    echo();
    mvwprintw(win, 2,2,"%s",question);
    move(5,3);
    wrefresh(win);
    char *str  = malloc(sizeof(char)*100);
    wgetstr(win, str);
    wrefresh(win);
    noecho();
    return str;
}