#ifndef _tui_component_menu_h
#define _tui_component_menu_h

#include <stdbool.h>
#include "ui/tui/window.h"

/*
 * 阻塞式菜单组件。
 *
 * 返回选中项下标 [0, count)；返回 -1 表示用户取消（q / Esc）。
 *
 * pre_info / post_info 用于在列表上方/下方绘制附加信息，
 * 签名返回"下一个可用的 y"，便于插入多行。
 */
typedef struct {
    const char *title;                    /* 可为 NULL */
    const char *const *items;             /* UTF-8 文案 */
    int count;
    int initial;                          /* 初始高亮下标 */
    int (*pre_info)(void *ctx, const ui_frame *f, int y);   /* 可为 NULL */
    int (*post_info)(void *ctx, const ui_frame *f, int y);  /* 可为 NULL */
    void *ctx;
    const char *hint;                     /* 状态栏提示，可为 NULL */
    bool enable_command;                  /* true 时按 / 返回 TUI_RESULT_COMMAND */
    bool center;                          /* true 时菜单整体水平居中 */
} tui_menu_spec;

int tui_menu(const tui_menu_spec *spec);

#endif // _tui_component_menu_h
