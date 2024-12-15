#ifndef SCREENS_H
#define SCREENS_H

//指代不同界面的变量
typedef enum {
    SCREEN_MAIN_MENU,
    SCREEN_1,
    SCREEN_2,
    SCREEN_3,
    SCREEN_4,
    SCREEN_5,
    SCREEN_6,
    SCREEN_EXIT,
    SCREEN_BACK
} Screen;

//
typedef struct {
    Screen screens[100];
    int top;
} Stack;


void push_screen(Stack *stack, Screen screen);
Screen pop_screen(Stack *stack);

//定义窗口的打印与处理的接口
typedef struct {
    void (*display)(void);  //显示界面函数的接口
    Screen (*input_handler)(Stack *stack);    //当前界面的处理函数,返回下一个界面
} WinProcess;

#endif