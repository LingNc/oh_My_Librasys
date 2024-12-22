#ifndef _func_h
#define _func_h

// 跨平台的一个禁用回显的标准输入
// Unix/Linux/macOS 使用 termios 禁用回显和标准输入缓冲
// Windows 使用 _getch()
char getch();
// 清空屏幕
void clear_screen();
// 处理退格键
void backspace(char *input, int *input_index);
void refresh(char *input, int *input_index, const char *new_content);
// 启用所有鼠标事件跟踪
void enable_mouse_tracking();
// 禁用鼠标事件跟踪
void disable_mouse_tracking();

#endif // _func_h