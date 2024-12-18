#include <stdio.h>
#include <stdlib.h>
#include "UI/screen.h"
#include "UI/menu.h"

void push_screen(Stack *stack, Screen screen)
{
    if (stack->top >= STACK_SIZE - 1)
    {
        fprintf(stderr, "屏幕栈已满\n");
        exit(EXIT_FAILURE);
    }
    stack->screens[++(stack->top)] = screen;
}

Screen pop_screen(Stack *stack)
{
    if (stack->top < 0)
    {
        fprintf(stderr, "屏幕栈为空\n");
        exit(EXIT_FAILURE);
    }
    return stack->screens[(stack->top)--];
}

//函数指针数组, 打印当前界面,处理选项,返回下一项
Screen (*screen_functions[])() = {
    [MAIN] = main_menu,
    [ADMIN_MENU] = admin_menu,
    [BOOK_MANAGE] = book_manage_info,
    [STUDENT_MANAGE] = stu_manage_info,
    [BORROW_RECORDS] = borrow_records,
    [STUDENT_MENU] = stu_menu,
   //[SEARCH_BOOKS] = search_books,直接使用借书界面
    [BORROW_BOOK] = borrow_records,
    [RETURN_BOOK] = return_book,
    [VIEW_RECORDS] = view_records,
    [ABOUT] = about,
    [BACK] = NULL,
    [EXIT] = NULL,
};

 