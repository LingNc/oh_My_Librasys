#ifndef _func_h
#define _func_h


// 跨平台的一个禁用回显的标准输入
// Unix/Linux/macOS 使用 termios 禁用回显和标准输入缓冲
// Windows 使用 _getch()
char getch();
// 清空屏幕
void clear_screen();

#endif // _func_h