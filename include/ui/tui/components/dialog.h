#ifndef _tui_component_dialog_h
#define _tui_component_dialog_h

#include <stdbool.h>

/* 阻塞式提示框：等待 Enter / 任意键关闭 */
void tui_info(const char *title, const char *fmt, ...);

/* 阻塞式错误框（红色） */
void tui_error(const char *title, const char *fmt, ...);

/* 阻塞式确认框，返回 true 表示确认 */
bool tui_confirm(const char *fmt, ...);

/* 非阻塞提示：在状态栏短暂显示后自动消失 */
void tui_toast(const char *fmt, ...);

#endif // _tui_component_dialog_h
