#include "utils.h"
#include "list.h"
#include "terminal.h"

void print_list(Book* books, WINDOW* pad, int i)
{ 
        mvwprintw(pad, i, 0,"%s",books[i].isbn);
        mvwprintw(pad, i, 20,"%s",books[i].name);
        mvwprintw(pad, i, 40,"%s",books[i].author);
        mvwprintw(pad, i, 50,"%s",books[i].publisher);
        mvwprintw(pad, i, 90,"%d",books[i].num);
}

void show_confirm_window(Book* book)
{
    WINDOW* win = newwin(terminal.height/2, terminal.width/2, terminal.height/4, terminal.width/4);
    box(win, 0,0);
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
    //set_menu_sub(menu, derwin(win, 1, 10, terminal.height * 3 / 4 - 1, terminal.height * 3 / 4 - 1));
    set_menu_format(menu, 1, 4);
    post_menu(menu);
    wrefresh(win);

    while(1)
    {   

        int ch = wgetch(win);
        switch(ch)
        {
            case KEY_LEFT:
                menu_driver(menu, REQ_LEFT_ITEM);
                break;
            case KEY_RIGHT:
                menu_driver(menu, REQ_RIGHT_ITEM);
                break;
            case 10:

                return;
        }
        wrefresh(win);
    }
}

void list()
{
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
        {"9787547008606", "电子技术", "刘伟", "华中科技大学出版社", 18},
        {"9787547008607", "土木工程", "张丽", "东南大学出版社", 19},
        {"9787547008608", "环境科学", "王强", "天津大学出版社", 20},
        {"9787547008609", "材料科学", "陈静", "重庆大学出版社", 21},
        {"9787547008610", "能源工程", "赵伟", "四川大学出版社", 22},
        {"9787547008611", "航空航天", "孙敏", "大连理工大学出版社", 23},
        {"9787547008612", "海洋科学", "周强", "中国海洋大学出版社", 24},
        {"9787547008613", "农业科学", "吴娟", "中国农业大学出版社", 25},
        {"9787547008614", "医学基础", "郑伟", "北京协和医学院出版社", 26},
        {"9787547008615", "药学概论", "林强", "中国药科大学出版社", 27},
        {"9787547008616", "护理学基础", "黄娟", "首都医科大学出版社", 28},
        {"9787547008617", "口腔医学", "徐伟", "四川大学华西口腔医学院出版社", 29},
        {"9787547008618", "中医学概论", "高娟", "北京中医药大学出版社", 30},
        {"9787547008619", "针灸推拿", "杨伟", "上海中医药大学出版社", 31},
        {"9787547008620", "康复医学", "刘娟", "广州中医药大学出版社", 32},
        {"9787547008621", "公共卫生", "张伟", "复旦大学公共卫生学院出版社", 33},
        {"9787547008622", "预防医学", "王强", "北京大学公共卫生学院出版社", 34},
        {"9787547008623", "临床医学", "陈娟", "清华大学医学院出版社", 35},
        {"9787547008624", "儿科学", "赵伟", "上海交通大学医学院出版社", 36},
        {"9787547008625", "妇产科学", "孙强", "浙江大学医学院出版社", 37},
        {"9787547008626", "神经科学", "周娟", "南京大学医学院出版社", 38},
        {"9787547008627", "精神医学", "吴伟", "武汉大学人民医院出版社", 39},
        {"9787547008628", "皮肤病学", "郑娟", "中山大学附属第一医院出版社", 40},
        {"9787547008629", "眼科学", "林伟", "厦门大学附属中山医院出版社", 41},
        {"9787547008630", "耳鼻喉科学", "黄强", "哈尔滨医科大学附属第一医院出版社", 42},
        {"9787547008631", "放射医学", "徐娟", "西安交通大学第一附属医院出版社", 43},
        {"9787547008632", "麻醉学", "高伟", "华中科技大学同济医学院出版社", 44},
        {"9787547008633", "急诊医学", "杨娟", "东南大学附属中大医院出版社", 45},
        {"9787547008634", "重症医学", "刘强", "天津医科大学总医院出版社", 46},
        {"9787547008635", "病理学", "张娟", "重庆医科大学附属第一医院出版社", 47},
        {"9787547008636", "检验医学", "王伟", "四川大学华西医院出版社", 48},
        {"9787547008637", "影像医学", "陈强", "大连医科大学附属第一医院出版社", 49},
        {"9787547008638", "核医学", "赵娟", "中国医科大学附属第一医院出版社", 50},
        {"9787547008639", "超声医学", "孙伟", "山东大学齐鲁医院出版社", 51},
        {"9787547008640", "介入医学", "周强", "青岛大学附属医院出版社", 52},
        {"9787547008641", "康复医学", "吴娟", "南京医科大学附属医院出版社", 53},
        {"9787547008642", "运动医学", "郑伟", "苏州大学附属第一医院出版社", 54},
        {"9787547008643", "营养学", "林强", "徐州医科大学附属医院出版社", 55},
        {"9787547008644", "老年医学", "黄娟", "南通大学附属医院出版社", 56},
        {"9787547008645", "儿科护理", "徐伟", "扬州大学附属医院出版社", 57},
        {"9787547008646", "妇产科护理", "高娟", "盐城师范学院出版社", 58},
        {"9787547008647", "内科护理", "杨伟", "淮阴师范学院出版社", 59},
        {"9787547008648", "外科护理", "刘娟", "南京晓庄学院出版社", 60},
        {"9787547008649", "急诊护理", "张伟", "江苏师范大学出版社", 61},
        {"9787547008650", "重症护理", "王强", "南京信息工程大学出版社", 62},
        {"9787547008651", "精神科护理", "陈娟", "南京工业大学出版社", 63},
        {"9787547008652", "肿瘤护理", "赵伟", "南京邮电大学出版社", 64},
        {"9787547008653", "康复护理", "孙强", "南京林业大学出版社", 65},
        {"9787547008654", "社区护理", "周娟", "南京农业大学出版社", 66},
};

    // 创建主窗口
    WINDOW *main_win = newwin(VISIBLE_ROWS + 2, PAD_WIDTH, 2, 10);  // 加上边框
    box(main_win, 0, 0);
    wrefresh(main_win);

    // 创建 pad
    WINDOW *pad = newpad(PAD_HEIGHT, PAD_WIDTH);

    // 绘制列表项到 pad 上
    for (int i = 1; i < LIST_SIZE + 3; i++) {
        print_list(books, pad, i);
        //mvwprintw(pad, i, 0, "%-*ls  %-*ls  %-*ls  %-*ls  %d", ISBN_WIDTH, books[i].isbn, BOOKNAME_WIDTH, books[i].name, AUTHOR_WIDTH, books[i].author,PUBLISHER_WIDTH, books[i].publisher, books[i].num );
    }

    int scroll_offset = 0;    // 滚动偏移量
    int current_row = 0;      // 当前选中的行

    // 高亮当前行
    wattron(pad, A_REVERSE);
    print_list(books, pad, current_row);
    //mvwprintw(pad, current_row, 0, "%-*ls  %-*ls  %-*ls  %-*ls  %d",ISBN_WIDTH, books[current_row].isbn, BOOKNAME_WIDTH, books[current_row].name, AUTHOR_WIDTH, books[current_row].author,PUBLISHER_WIDTH, books[current_row].publisher, books[current_row].num );
    wattroff(pad, A_REVERSE);
    
    init_pair(3, COLOR_BLUE, COLOR_CYAN);
    attron(COLOR_PAIR(3));
    mvwprintw(main_win, 0, 5, "%s", "ISBN");
    mvwprintw(main_win, 0, 23, "%s", "书名");
    mvwprintw(main_win, 0, 40, "%s", "作者");
    mvwprintw(main_win, 0, 55, "%s", "出版社");
    mvwprintw(main_win, 0, 90, "%s", "数量");
    attroff(COLOR_PAIR(3));


    // 刷新 pad 的初始视图
    prefresh(pad, scroll_offset, 0, 3,11, VISIBLE_ROWS + 1, PAD_WIDTH - 1);
    wrefresh(main_win);
    while (1) {
        // 获取用户输入
        int ch = getch();
        switch (ch) {
            case KEY_UP:  // 上箭头
                if (current_row > 0) {
                    // 去掉旧的高亮
                    print_list(books, pad, current_row);
                    //mvwprintw(pad, current_row, 0, "%-*ls  %-*ls  %-*ls  %-*ls  %d",ISBN_WIDTH, books[current_row].isbn, BOOKNAME_WIDTH, books[current_row].name, AUTHOR_WIDTH, books[current_row].author,PUBLISHER_WIDTH, books[current_row].publisher, books[current_row].num );
                    current_row--;
                    if (current_row < scroll_offset && scroll_offset > 0) {
                        scroll_offset--;
                    }
                }
                break;
            case KEY_DOWN:  // 下箭头
                if (current_row < LIST_SIZE - 1) {
                    // 去掉旧的高亮
                    print_list(books, pad, current_row);
                    //mvwprintw(pad, current_row, 0, "%-*ls  %-*ls  %-*ls  %-*ls  %d",ISBN_WIDTH, books[current_row].isbn, BOOKNAME_WIDTH, books[current_row].name, AUTHOR_WIDTH, books[current_row].author,PUBLISHER_WIDTH, books[current_row].publisher, books[current_row].num );
                    current_row++;
                    if (current_row >= scroll_offset + VISIBLE_ROWS && scroll_offset + VISIBLE_ROWS < PAD_HEIGHT) {
                        scroll_offset++;
                    }
                }
                break;
            case 10:  // 回车键
                // 弹出确认窗口
                show_confirm_window(&books[current_row]);
                break;
            case 'q':  // 按 'q' 退出
                return;
        }

        // 设置新的高亮
        wattron(pad, A_REVERSE);
        print_list(books, pad, current_row);
        //mvwprintw(pad, current_row, 0, "%-*ls  %-*ls  %-*ls  %-*ls  %d",ISBN_WIDTH, books[current_row].isbn, BOOKNAME_WIDTH, books[current_row].name, AUTHOR_WIDTH, books[current_row].author,PUBLISHER_WIDTH, books[current_row].publisher, books[current_row].num );
        wattroff(pad, A_REVERSE);

        // 刷新 pad 的视图
        prefresh(pad, scroll_offset, 0, 3, 11, VISIBLE_ROWS + 1, PAD_WIDTH - 1);
    }
}
