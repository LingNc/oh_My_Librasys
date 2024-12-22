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

void display_page(vector content, int page, int total_pages, int highlight, const char *prefix, void *args[]) {
    clear_screen();
    if (prefix) {
        printf("%s\n", prefix);
    }
    *(int *)args[1] = *(int *)args[0];
    if ((int)content->size(content) < *(int *)args[1]) {
        *(int *)args[1] = (int)content->size(content);
    }
    display(highlight, content);
    printf("\nPage: %d / %d\n", page + 1, total_pages);
}

bool handle_page_input(int *page, int total_pages, int *highlight, int *choice, void **args) {
    char ch = getch();
    bool direct_jump = false;
    int pageSize = *(int *)args[0];
    int lineSize = *(int *)args[1];
    int max_highlight = pageSize - 1;

    switch (ch) {
    case 'a':
    case 'h':
    case 'p':
        if (*page > 0) {
            *page = (*page - 1) % total_pages; // 向前翻页
            *highlight = 0; // 重置光标位置
        }
        break;
    case 'd':
    case 'l':
    case 'n':
        if (*page < total_pages - 1) {
            *page = (*page + 1) % total_pages; // 向后翻页
            *highlight = 0; // 重置光标位置
        }
        break;
    case 'w':
        *highlight = (*highlight - 1) % lineSize; // 上移光标
        break;
    case 's':
        *highlight = (*highlight + 1) % lineSize; // 下移光标
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
            *highlight = ((ch - '0') - 1) % pageSize;
            direct_jump = true;
        }
        break;
    }

    return direct_jump || ch == '\n';
}

void page(dataBase db, int pageSize, const char *prefix, void (*func)(void *)) {
    int total_pages = (db->size(db) + pageSize - 1) / pageSize;
    int page = 0;
    int highlight = 0;
    int choice = -1;
    int lineSize = 0;
    void *args[] = { &pageSize, &lineSize };
    while (1) {
        vector content = db->get(db, page * pageSize, pageSize);
        display_page(content, page, total_pages, highlight, prefix, args);
        bool res = handle_page_input(&page, total_pages, &highlight, &choice, args);
        if (choice != -1 && res) {
            if (choice > total_pages) {
                printf("无效选择，请重新选择\n");
                getchar();
            } else {
                void *item=content->at(content,choice);
                // 传入选中的结构体指针
                func(item);
            }
            // 重置选择
            choice=-1;
        }
        content->free(content);
        total_pages = (db->size(db) + pageSize - 1) / pageSize; // 更新总页数
    }
}