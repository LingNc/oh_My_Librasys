#ifndef _tui_h
#define _tui_h

#include <stdbool.h>

/*
 * TUI (ncursesw) 生命周期。
 *
 * tui_init()   : 初始化终端（setlocale / initscr / cbreak / noecho / keypad）
 * tui_shutdown(): 恢复终端，可重复调用；已注册 atexit 兜底
 * tui_run()    : TUI 主入口，返回进程退出码
 */
bool tui_init(void);
void tui_shutdown(void);
int  tui_run(void);

#endif // _tui_h
