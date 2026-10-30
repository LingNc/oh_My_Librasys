#ifndef _tui_component_prompt_h
#define _tui_component_prompt_h

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    TUI_PROMPT_OK = 0,
    TUI_PROMPT_CANCEL
} tui_prompt_result;

/*
 * Tab 补全回调：拿到当前文本，写入补全结果到 out。
 * out 与输入相同表示无补全。
 */
typedef void (*tui_prompt_complete_fn)(const char *text, char *out, size_t cap);

/*
 * 底部单行输入框（命令模式）。
 *   title   : 左侧提示，可为 NULL
 *   initial : 初始文本，可为 NULL
 *   buf/cap : 调用方缓冲区（UTF-8）
 *   complete: 可为 NULL
 */
tui_prompt_result tui_prompt(const char *title, const char *initial,
                             char *buf, size_t cap,
                             tui_prompt_complete_fn complete);

#endif // _tui_component_prompt_h
