#ifndef _tui_component_table_h
#define _tui_component_table_h

#include <stdbool.h>
#include <stddef.h>
#include "ui/tui/window.h"

/*
 * 阻塞式分页表格。
 *
 * 数据通过回调按需取，避免一次性加载全库：
 *   row_count(ctx)                     → 总行数
 *   row_text(ctx, row, col, out, cap)  → 第 row 行第 col 列的 UTF-8 文本
 *
 * 返回绝对行号；返回 -1 表示取消。
 *
 * 交互：↑↓/jk 移动  ←→/hl/PgUp/PgDn 翻页  Enter 选中  q 返回  数字直选
 */
typedef struct {
    const char *title;
    const char *const *headers;      /* 表头，可为 NULL */
    const int *col_widths;           /* 每列显示列宽 */
    size_t ncols;

    size_t (*row_count)(void *ctx);
    void   (*row_text)(void *ctx, size_t row, size_t col, char *out, size_t cap);
    void  *ctx;

    size_t page_size;                /* 0 → 默认 10 */
    const char *hint;                /* 状态栏提示，可为 NULL */
    bool enable_command;             /* true 时按 / 返回 TUI_RESULT_COMMAND */

    /* 选中当前行后调用；返回 true 表示继续留在表格，false 表示返回调用方 */
    bool (*on_enter)(void *ctx, size_t row);
} tui_table_spec;

long tui_table(const tui_table_spec *spec);

/* 全局默认每页行数（/set 命令使用） */
size_t tui_table_default_page_size(void);
void tui_table_set_default_page_size(size_t size);

/* 下一次重绘时跳转到指定页（0 起），/page 命令使用 */
void tui_table_request_page(size_t page);

#endif // _tui_component_table_h
