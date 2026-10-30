#ifndef _tui_view_command_h
#define _tui_view_command_h

#include <stdbool.h>

typedef enum {
    TUI_CMD_NONE = 0,
    TUI_CMD_SEARCH_BOOKS,
    TUI_CMD_SEARCH_STUDENTS
} tui_command_kind;

typedef struct {
    tui_command_kind kind;
    char argument[128];
} tui_command_request;

/*
 * 打开命令面板。
 *   /help            命令帮助
 *   /search <关键字>  搜索图书（学生控制台内搜索学生请用 /students）
 *   /students <关键字> 搜索学生
 *   /set <n>         设置表格每页行数
 *   /page <n>        跳转到指定页
 *
 * 产生搜索请求时返回 true 并填充 request，调用方负责打开结果视图。
 */
bool tui_command_palette(tui_command_request *request);

/* 搜索结果视图 */
void tui_show_book_search(const char *keyword);
void tui_show_student_search(const char *keyword);

/* 打开命令面板并分发搜索请求（供各视图在收到 TUI_RESULT_COMMAND 时调用） */
void tui_handle_command(void);

#endif // _tui_view_command_h
