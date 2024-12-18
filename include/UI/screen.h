//用于管理界面栈的相关函数与结构

#ifndef SCREENS_H
#define SCREENS_H


#define STACK_SIZE 100
//指代不同界面的变量
typedef enum
{
    // 主界面
    MAIN, // 主界面
    ADMIN_LOGIN,    // 管理员登录
    STUDENT_LOGIN, // 学生登录
    ABOUT,          //关于

    // 管理员界面
    ADMIN_MENU,     // 管理员主界面
    BOOK_MANAGE,    // 图书管理*
    STUDENT_MANAGE, // 学生管理*
    BORROW_RECORDS, // 借书记录管理*

    // 学生界面
    STUDENT_MENU,  // 学生主界面
    SEARCH_BOOKS,  // 搜索图书*
    BORROW_BOOK,   // 借书*
    RETURN_BOOK,   // 还书*
    VIEW_RECORDS,  // 查看借书记录*

    // 通用界面
    CONFIRM,     // 确认界面
    SUCCESS,     // 操作成功提示
    ERROR,       // 操作失败提示
    BACK,         // 返回
    EXIT,        //退出
} Screen;

//界面栈的定义
typedef struct {
    Screen screens[100];
    int top;
} Stack;


void push_screen(Stack *stack, Screen screen);
Screen pop_screen(Stack *stack);


#endif