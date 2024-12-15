#include "screen.h"
#include <stdio.h>

#include "menu.h"

void push_screen(Stack *stack, Screen screen)
{
    stack ->top++;
    if (stack->top > 99)
    {
        perror("视频栈已满");
        exit(1);
    }
    stack -> screens[stack -> top] = screen;
}

Screen pop_screen(Stack *stack)
{
    stack -> top--;
}

WinProcess wins[] = {
    [SCREEN_MAIN_MENU] = {display_main_menu, main_menu_input},
    [SCREEN_1] = {option1_display, option1_input},
    [SCREEN_2] = {option2_display, option2_input},
    // 其他界面...
};