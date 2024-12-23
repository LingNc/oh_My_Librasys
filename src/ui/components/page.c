#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "ui/components/page.h"
#include "ui/components/list.h"
#include "ui/components/func.h"
#include "ui/command.h"
#include "DataBase/DataBase.h"
#include "Tools/Vector.h"
#include "Tools/String.h"
#include "models/Book.h"
#include "function.h"


void display_page(vector content, int page, int total_pages, int highlight, void *args[]) {
    *(int *)args[1] = *(int *)args[0];
    if ((int)content->size(content) < *(int *)args[1]) {
        *(int *)args[1] = (int)content->size(content);
    }
    display(highlight, content);
    printf("\nPage: %d / %d\n", page + 1, total_pages);
}

int handle_page_input(int *page, int total_pages, int *highlight, int *choice, void **args) {
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
        *page = *(int *)args[2]; // 更新页码
        *highlight = 0; // 重置光标位置
        break;
    default:
        if (ch >= '0' && ch <= '9') {
            *highlight = ((ch - '0') - 1) % pageSize;
            direct_jump = true;
        }
        break;
    }
    if(direct_jump||ch=='\n') return 1;
    else if(ch=='q') return -1;
    else return 0;
}

void page(dataBase db, int pageSize, void (**funcs)(void *), void **arg) {
    int total_pages = (db->size(db) + pageSize - 1) / pageSize;
    int page = 0;
    int highlight = 0;
    int choice = -1;
    int lineSize = 0;
    void *args[] = { &pageSize, &lineSize, &page, &total_pages };
    while(1){
        vector content=new_vector(db->_type->c_str(db->_type));
        // 是否显示已借书籍
        if(*(bool *)arg[3]){
            dataBase borrowDb=arg[4];
            vector temp=load_borrow_records(borrowDb,((student)arg[0])->id);
            for(size_t i=0; i<temp->size(temp); ++i){
                string t=(string)temp->at(temp,i);
                size_t book_id;
                memcpy(&book_id,t->c_str(t),sizeof(size_t));
                book b=db->find_key(db,book_id);
                if(b) content->push_back(content,b);
                // size_t timestamp;
                // memcpy(&timestamp,t->c_str(t)+sizeof(size_t),sizeof(size_t));
                // 查看时间无法显示比较复杂，暂时不显示
            }
        }
        else{
            content=db->get(db,page*pageSize,pageSize);
        }
        clear_screen();
        // preInfo
        if (funcs[0]){
            if(arg){
                if(arg[0]) funcs[0](arg[0]);
                else funcs[0](NULL);
            }
            else funcs[0](NULL);
        }
        display_page(content, page, total_pages, highlight, args);
        // postInfo
        if (funcs[2]){
            if(arg){
                if(arg[0]) funcs[2](arg[0]);
                else funcs[2](NULL);
            }
            else funcs[2](NULL);
        }
        int res=handle_page_input(&page,total_pages,&highlight,&choice,args);
        // 退出
        if(res==-1) break;
        if(choice!=-1&&res==1){
            if (choice > content->size(content)+1) {
                printf("无效选择，请重新选择\n");
                getchar();
            }
            else{
                struct{
                    void *from;
                    void *item;
                }*itemArgs=malloc(sizeof(*itemArgs));
                itemArgs->item=content->at(content,choice-1);
                itemArgs->from=arg[0];
                // 传入选中的结构体指针
                funcs[1](itemArgs);
            }
            // 重置选择
            choice = -1;
        }
        content->free(content);
        total_pages = (db->size(db) + pageSize - 1) / pageSize; // 更新总页数
    }
}