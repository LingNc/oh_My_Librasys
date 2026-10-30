#ifndef _ui_window_h
#define _ui_window_h

#include <stdbool.h>
#include <wchar.h>

/* 屏幕最小尺寸（小于此值时只显示提示） */
#define UI_MIN_COLS 60
#define UI_MIN_ROWS 16

/* 组件返回的特殊值：用户按 / 请求打开命令面板 */
#define TUI_RESULT_COMMAND (-2)

/*
 * 帧布局：
 *   ┌──────────────────────────────┐  第 0 行：标题栏
 *   │  内容区 (y0..y1-1, x0..x1-1) │
 *   ├──────────────────────────────┤  第 rows-1 行：状态栏
 *   └──────────────────────────────┘
 */
typedef struct {
    int rows, cols;      /* 屏幕尺寸 */
    int y0, x0;          /* 内容区左上（含） */
    int y1, x1;          /* 内容区右下（不含） */
    int w, h;            /* 内容区列数 / 行数 */
} ui_frame;

/* 清屏 + 画外框。返回 false 表示屏幕太小，已显示提示。
 * title / status 可为 NULL。 */
bool ui_frame_begin(const char *title, const char *status, ui_frame *f);

/* 提交刷新（wnoutrefresh + doupdate），避免闪烁 */
void ui_frame_end(void);

/* 文本绘制：按列宽裁剪，maxw <= 0 表示自动到屏幕右边界 */
void ui_puts_w(int y, int x, int maxw, const wchar_t *s, int attr);
void ui_puts(int y, int x, int maxw, const char *utf8, int attr);
void ui_puts_center_w(int y, int x, int maxw, const wchar_t *s, int attr);
void ui_puts_center(int y, int x, int maxw, const char *utf8, int attr);

/* 用空格填满一行的一段，用于擦除高亮残留 */
void ui_clear_span(int y, int x, int cols);

/* 横向重复字符（画分隔线） */
void ui_hline_w(int y, int x, int cols, wchar_t ch);

#endif // _ui_window_h
