#ifndef _tui_component_form_h
#define _tui_component_form_h

#include <stdbool.h>
#include <stddef.h>

/* 字段标志 */
enum {
    TUI_FIELD_MASKED    = 1 << 0,   /* 密码：显示 * */
    TUI_FIELD_NUMERIC   = 1 << 1,   /* 只接受数字 */
};

/*
 * 单行输入字段。
 *   buf  : 调用方提供的 UTF-8 缓冲区，表单成功时写入
 *   cap  : 缓冲区容量（字节）
 *   validate: 返回 NULL 表示通过，否则返回错误提示（静态字符串）
 */
typedef struct {
    const char *label;                     /* UTF-8 */
    char       *buf;
    size_t      cap;
    unsigned    flags;
    const char *(*validate)(const char *value);
} tui_field;

typedef enum {
    TUI_FORM_OK = 0,
    TUI_FORM_CANCEL
} tui_form_result;

/*
 * 阻塞式表单。
 *   title: 框标题
 *   submithint: 状态栏提示，可为 NULL
 * 返回 TUI_FORM_OK 时所有 buf 已写入（UTF-8）；CANCEL 时内容可能已改动，调用方自行忽略。
 */
tui_form_result tui_form(const char *title, tui_field *fields, size_t nfields,
                         const char *submithint);

#endif // _tui_component_form_h
