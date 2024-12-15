#include <stdio.h>
#include <stdlib.h>
#include "screen.h"
#include "menu.h"

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
    [BACK] = NULL,
    [EXIT] = NULL,
};

 