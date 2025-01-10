#ifndef _func_h
#define _func_h

#include <stdbool.h>
#include <time.h>
#define MAX_BORROW_TIME (30*86400)


// 跨平台的一个禁用回显的标准输入
// Unix/Linux/macOS 使用 termios 禁用回显和标准输入缓冲
// Windows 使用 _getch()
char getch();
// 清空屏幕
void clear_screen();
// 无回显的整行读入，返回0退出，1正常
bool getaline(char str[],char eof[]);
// 处理退格键
void backspace(char *input, int *input_index);
void refresh(char *input, int *input_index, const char *new_content);
// 启用所有鼠标事件跟踪
void enable_mouse_tracking();
// 禁用鼠标事件跟踪
void disable_mouse_tracking();
// 时间戳转化函数
const char* str_time(const char* format, time_t timestamp);

#define MAX_INPUT 100

#endif // _func_h