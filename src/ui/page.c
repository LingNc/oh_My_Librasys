#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "ui/page.h"
#include "ui/list.h"
#include "DataBase/DataBase.h"
#include "Tools/Vector.h"
#include "ui/func.h"
#include "ui/command.h"

void display_page(vector content, int highlight, const char prefix[]) {
    clear_screen();
    printf("%s\n", prefix);
    display(highlight, content);
    printf("Page: %d\n", highlight + 1);
}

bool handle_page_input(int *page, int total_pages, int *highlight, int *choice, void **args) {
    int *page_size = (int *)args[0];
    char ch = getch();
    bool direct_jump = false;

    switch (ch) {
    case 'a':
    case 'h':
    case 'p':
        *page = (*page - 1 + total_pages) % total_pages; // 向前翻页
        *highlight = 0; // 重置光标位置
        break;
    case 'd':
    case 'l':
    case 'n':
        *page = (*page + 1) % total_pages; // 向后翻页
        *highlight = 0; // 重置光标位置
        break;
    case 'w':
        *highlight = (*highlight - 1 + total_pages) % total_pages; // 上移光标
        break;
    case 's':
        *highlight = (*highlight + 1) % total_pages; // 下移光标
        break;
    case '\n':
        *choice = *highlight + 1;
        break;
    case '/':
        execute(args);
        *page = 0; // 重置页码
        *highlight = 0; // 重置光标位置
        break;
    default:
        if (ch >= '0' && ch <= '9') {
            *highlight = ((ch - '0') - 1) % total_pages;
            direct_jump = true;
        }
        break;
    }

    return direct_jump || ch == '\n';
}

void page(dataBase db, int page_size, const char *prefix) {
    int total_pages = (db->size(db) + page_size - 1) / page_size;
    int page = 0;
    int highlight = 0;
    int choice = -1;
    void *args[] = { &page_size };
    while (1) {
        vector content = db->get(db, page * page_size, page_size);
        display_page(content, highlight, prefix);
        bool res = handle_page_input(&page, total_pages, &highlight, &choice, args);
        if (choice != -1 && res) {
            if (choice > total_pages) {
                printf("无效选择，请重新选择\n");
                getchar();
            }
            choice = -1; // 重置选择
        }
        content->free(content);
        total_pages = (db->size(db) + page_size - 1) / page_size; // 更新总页数
    }
}