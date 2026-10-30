#include "ui/tui/views/command.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service/admin.h"
#include "service/search.h"
#include "ui/tui/components/dialog.h"
#include "ui/tui/components/prompt.h"
#include "ui/tui/components/table.h"

#define COMMAND_COUNT 5

static const char *const commands[COMMAND_COUNT] = {
    "help",
    "search",
    "students",
    "set",
    "page",
};

/* 复用的命令 Trie（懒初始化） */
typedef struct command_node {
    struct command_node *children[128];
    int is_end;
} command_node;

static command_node root;
static bool root_ready = false;

static void trie_build(void) {
    if (root_ready) return;
    memset(&root, 0, sizeof(root));

    for (int i = 0; i < COMMAND_COUNT; ++i) {
        command_node *node = &root;
        for (const char *p = commands[i]; *p; ++p) {
            unsigned char index = (unsigned char)*p;
            if (!node->children[index]) {
                command_node *child = calloc(1, sizeof(*child));
                if (!child) return;
                node->children[index] = child;
            }
            node = node->children[index];
        }
        node->is_end = 1;
    }
    root_ready = true;
}

static int collect_words(command_node *node, char *buffer, size_t depth,
                         const char **out, int count, int max) {
    if (count >= max) return count;
    if (node->is_end) {
        buffer[depth] = '\0';
        out[count++] = strdup(buffer);
        if (count >= max) return count;
    }
    for (int i = 0; i < 128; ++i) {
        if (!node->children[i]) continue;
        buffer[depth] = (char)i;
        count = collect_words(node->children[i], buffer, depth + 1, out, count, max);
        if (count >= max) return count;
    }
    return count;
}

static void complete_command(const char *text, char *out, size_t cap) {
    out[0] = '\0';
    if (!text || !text[0]) return;
    trie_build();

    /* 只补全第一个词 */
    size_t word_len = 0;
    while (text[word_len] && !isspace((unsigned char)text[word_len])) ++word_len;
    if (word_len == 0) return;

    char word[128];
    if (word_len >= sizeof(word)) return;
    memcpy(word, text, word_len);
    word[word_len] = '\0';

    command_node *node = &root;
    for (size_t i = 0; i < word_len; ++i) {
        unsigned char index = (unsigned char)word[i];
        if (!node->children[index]) return;
        node = node->children[index];
    }

    /* 唯一分支则继续展开 */
    while (!node->is_end) {
        int only = -1, children = 0;
        for (int i = 0; i < 128; ++i) {
            if (node->children[i]) { ++children; only = i; }
        }
        if (children != 1) break;
        if (word_len + 1 >= sizeof(word)) break;
        word[word_len++] = (char)only;
        word[word_len] = '\0';
        node = node->children[only];
    }

    if (node->is_end) {
        snprintf(out, cap, "%s", word);
        return;
    }

    /* 多分支：列出候选 */
    char buffer[128];
    memcpy(buffer, word, word_len + 1);
    const char *matches[16];
    int count = collect_words(node, buffer, word_len, matches, 0, 16);
    if (count > 0) {
        char suggestion[512];
        suggestion[0] = '\0';
        for (int i = 0; i < count; ++i) {
            if (i) strncat(suggestion, "  ", sizeof(suggestion) - strlen(suggestion) - 1);
            strncat(suggestion, matches[i], sizeof(suggestion) - strlen(suggestion) - 1);
            free((void *)matches[i]);
        }
        snprintf(out, cap, "%s", word);
        tui_toast("候选：%s", suggestion);
    }
}

static void show_help(void) {
    tui_info("命令帮助",
             "/search <关键字>    搜索图书\n"
             "/students <关键字>  搜索学生\n"
             "/set <n>            设置每页行数\n"
             "/page <n>           跳转到第 n 页\n"
             "/help               显示本帮助\n"
             "Tab 补全命令，Esc 取消");
}

bool tui_command_palette(tui_command_request *request) {
    if (!request) return false;
    request->kind = TUI_CMD_NONE;
    request->argument[0] = '\0';

    char buffer[256] = "";
    if (tui_prompt("命令", "", buffer, sizeof(buffer), complete_command) != TUI_PROMPT_OK) {
        return false;
    }

    /* 允许用户输入前导 / */
    char *text = buffer;
    while (*text == ' ' || *text == '/') ++text;

    char name[64] = "";
    char argument[128] = "";
    int consumed = 0;
    if (sscanf(text, "%63s%n", name, &consumed) != 1) return false;

    const char *rest = text + consumed;
    while (*rest == ' ') ++rest;
    snprintf(argument, sizeof(argument), "%s", rest);

    if (strcmp(name, "help") == 0) {
        show_help();
        return false;
    }
    if (strcmp(name, "search") == 0) {
        if (!argument[0]) {
            tui_error("命令", "用法：/search <关键字>");
            return false;
        }
        request->kind = TUI_CMD_SEARCH_BOOKS;
        snprintf(request->argument, sizeof(request->argument), "%s", argument);
        return true;
    }
    if (strcmp(name, "students") == 0) {
        if (!argument[0]) {
            tui_error("命令", "用法：/students <关键字>");
            return false;
        }
        request->kind = TUI_CMD_SEARCH_STUDENTS;
        snprintf(request->argument, sizeof(request->argument), "%s", argument);
        return true;
    }
    if (strcmp(name, "set") == 0) {
        int size = atoi(argument);
        if (size <= 0) {
            tui_error("命令", "/set 需要一个正整数");
            return false;
        }
        tui_table_set_default_page_size((size_t)size);
        tui_toast("每页行数已设为 %d", size);
        return false;
    }
    if (strcmp(name, "page") == 0) {
        int page = atoi(argument);
        if (page <= 0) {
            tui_error("命令", "/page 需要一个以 1 开始的正整数");
            return false;
        }
        tui_table_request_page((size_t)(page - 1));
        tui_toast("已跳转到第 %d 页", page);
        return false;
    }

    tui_error("未知命令", "/%s 不存在，输入 /help 查看帮助", name);
    return false;
}

void tui_handle_command(void) {
    tui_command_request request;
    if (!tui_command_palette(&request)) return;

    switch (request.kind) {
    case TUI_CMD_SEARCH_BOOKS:
        tui_show_book_search(request.argument);
        break;
    case TUI_CMD_SEARCH_STUDENTS:
        tui_show_student_search(request.argument);
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------- 搜索结果 */

typedef struct {
    search_keys found;
    size_t cached_row;
    void *cached_item;
    bool is_book;
} search_context;

static void search_forget(search_context *ctx) {
    if (!ctx->cached_item) return;
    if (ctx->is_book) free_book(ctx->cached_item);
    else free_student(ctx->cached_item);
    ctx->cached_item = NULL;
    ctx->cached_row = (size_t)-1;
}

static void *search_row_item(search_context *ctx, size_t row) {
    if (ctx->cached_item && ctx->cached_row == row) return ctx->cached_item;
    search_forget(ctx);
    if (row >= ctx->found.count) return NULL;

    size_t key = ctx->found.keys[row];
    ctx->cached_item = ctx->is_book ? (void *)admin_get_book(key)
                                    : (void *)admin_get_student(key);
    ctx->cached_row = row;
    return ctx->cached_item;
}

static size_t search_rows(void *ctx) {
    return ((search_context *)ctx)->found.count;
}

static void search_book_row(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    book item = search_row_item(ctx, row);
    if (!item) { out[0] = '\0'; return; }
    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->id); break;
    case 1: snprintf(out, cap, "%s", item->ISBN->c_str(item->ISBN)); break;
    case 2: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 3: snprintf(out, cap, "%s", item->author->c_str(item->author)); break;
    default: snprintf(out, cap, "%s", item->status ? "已借出" : "可借"); break;
    }
}

static void search_student_row(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    student item = search_row_item(ctx, row);
    if (!item) { out[0] = '\0'; return; }
    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->studentID); break;
    case 1: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 2: snprintf(out, cap, "%s", item->class->c_str(item->class)); break;
    case 3: snprintf(out, cap, "%s", item->department->c_str(item->department)); break;
    default: snprintf(out, cap, "%d", item->borrowedCount); break;
    }
}

void tui_show_book_search(const char *keyword) {
    search_context ctx = { .found = search_books(keyword, SEARCH_DEFAULT_LIMIT),
                           .cached_row = (size_t)-1, .cached_item = NULL, .is_book = true };
    if (ctx.found.count == 0) {
        search_keys_free(&ctx.found);
        tui_info("搜索图书", "没有匹配「%s」的图书。", keyword);
        return;
    }

    static const char *const headers[] = { "ID", "ISBN", "书名", "作者", "状态" };
    static const int widths[] = { 8, 18, 28, 18, 10 };
    tui_table_spec spec = {
        "搜索结果 · 图书", headers, widths, 5,
        search_rows, search_book_row, &ctx, 0,
        "↑↓/jk 移动  q 返回  / 命令", true, NULL,
    };
    tui_table(&spec);

    search_forget(&ctx);
    bool truncated = ctx.found.truncated;
    search_keys_free(&ctx.found);
    if (truncated) tui_toast("结果超过 %d 条，仅显示前 %d 条", SEARCH_DEFAULT_LIMIT, SEARCH_DEFAULT_LIMIT);
}

void tui_show_student_search(const char *keyword) {
    search_context ctx = { .found = search_students(keyword, SEARCH_DEFAULT_LIMIT),
                           .cached_row = (size_t)-1, .cached_item = NULL, .is_book = false };
    if (ctx.found.count == 0) {
        search_keys_free(&ctx.found);
        tui_info("搜索学生", "没有匹配「%s」的学生。", keyword);
        return;
    }

    static const char *const headers[] = { "学号", "姓名", "班级", "学院", "已借" };
    static const int widths[] = { 14, 18, 20, 24, 8 };
    tui_table_spec spec = {
        "搜索结果 · 学生", headers, widths, 5,
        search_rows, search_student_row, &ctx, 0,
        "↑↓/jk 移动  q 返回  / 命令", true, NULL,
    };
    tui_table(&spec);

    search_forget(&ctx);
    bool truncated = ctx.found.truncated;
    search_keys_free(&ctx.found);
    if (truncated) tui_toast("结果超过 %d 条，仅显示前 %d 条", SEARCH_DEFAULT_LIMIT, SEARCH_DEFAULT_LIMIT);
}
