#include "ui/tui/views/student_borrow.h"

#include <stdio.h>

#include "DataBase/DataBase.h"
#include "service/admin.h"          /* 通用目录键列表：admin_*_keys / admin_get_* */
#include "service/borrow.h"
#include "ui/tui/components/dialog.h"
#include "ui/tui/components/table.h"
#include "ui/tui/views/command.h"

static void run_command(void *ctx) {
    (void)ctx;
    tui_handle_command();
}

/* ------------------------------------------------------------------ 借书 */

typedef struct {
    admin_keys keys;
    size_t cached_row;
    book cached_item;
    student user;
} borrow_context;

static void borrow_forget(borrow_context *ctx) {
    if (ctx->cached_item) free_book(ctx->cached_item);
    ctx->cached_item = NULL;
    ctx->cached_row = (size_t)-1;
}

static book borrow_row_item(borrow_context *ctx, size_t row) {
    if (ctx->cached_item && ctx->cached_row == row) return ctx->cached_item;
    borrow_forget(ctx);
    if (row >= ctx->keys.count) return NULL;
    ctx->cached_item = admin_get_book(ctx->keys.keys[row]);
    ctx->cached_row = row;
    return ctx->cached_item;
}

static size_t borrow_rows(void *ctx) {
    return ((borrow_context *)ctx)->keys.count;
}

static void borrow_row_text(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    book item = borrow_row_item(ctx, row);
    if (!item) { out[0] = '\0'; return; }

    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->id); break;
    case 1: snprintf(out, cap, "%s", item->ISBN->c_str(item->ISBN)); break;
    case 2: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 3: snprintf(out, cap, "%s", item->author->c_str(item->author)); break;
    default:
        if (item->status == 0) {
            snprintf(out, cap, "可借");
        } else {
            size_t days = 0, hours = 0;
            if (borrow_overdue_info(item, &days, &hours)) {
                snprintf(out, cap, "已借出 逾期%zu天", days);
            } else {
                snprintf(out, cap, "已借出");
            }
        }
        break;
    }
}

static bool borrow_action(void *ctx, size_t row) {
    borrow_context *list = ctx;
    book item = borrow_row_item(list, row);
    if (!item) return false;

    if (item->status == 1) {
        tui_error("无法借阅", "《%s》已被借出。", item->name->c_str(item->name));
        return false;
    }
    if (!tui_confirm("确认借阅《%s》？", item->name->c_str(item->name))) return false;

    borrow_result result = borrow_student_book(list->user, item);
    if (result.status == BORROW_OK) tui_info("借书成功", "%s", result.message);
    else tui_error("借书失败", "%s", result.message);
    return false;   /* 退出快照并刷新列表 */
}

void tui_student_borrow(student user) {
    for (;;) {
        borrow_context ctx = { admin_book_keys(), (size_t)-1, NULL, user };
        static const char *const headers[] = { "ID", "ISBN", "书名", "作者", "状态" };
        static const int widths[] = { 8, 18, 28, 18, 18 };

        tui_table_spec spec = {
            .title = "借书 · 选择图书", .headers = headers, .col_widths = widths, .ncols = 5,
            .row_count = borrow_rows, .row_text = borrow_row_text, .ctx = &ctx, .page_size = 0,
            .hint = "↑↓/jk 移动  ←→/hl 翻页  Enter 借阅  q 返回", .enable_command = true,
            .on_command = run_command,
            .on_enter = borrow_action,
        };

        long selected = tui_table(&spec);
        borrow_forget(&ctx);
        admin_keys_free(&ctx.keys);
        if (selected == TUI_RESULT_COMMAND) { tui_handle_command(); continue; }
        if (selected < 0) return;
    }
}

/* ------------------------------------------------------------------ 还书 */

typedef struct {
    vector books;
    student user;
} return_context;

static size_t return_rows(void *ctx) {
    return ((return_context *)ctx)->books->size(((return_context *)ctx)->books);
}

static void return_row_text(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    return_context *list = ctx;
    book item = list->books->at(list->books, row);
    if (!item) { out[0] = '\0'; return; }

    switch (col) {
    case 0: snprintf(out, cap, "%zu", item->id); break;
    case 1: snprintf(out, cap, "%s", item->name->c_str(item->name)); break;
    case 2: snprintf(out, cap, "%s", item->author->c_str(item->author)); break;
    default: {
        size_t days = 0, hours = 0;
        if (borrow_overdue_info(item, &days, &hours)) {
            snprintf(out, cap, "已逾期 %zu天%zu小时", days, hours);
        } else {
            snprintf(out, cap, "借阅中");
        }
        break;
    }
    }
}

static bool return_action(void *ctx, size_t row) {
    return_context *list = ctx;
    book item = list->books->at(list->books, row);
    if (!item) return false;
    if (list->books->size(list->books) == 0) return false;

    size_t days = 0, hours = 0;
    bool overdue = borrow_overdue_info(item, &days, &hours);

    char prompt[512];
    if (overdue) {
        snprintf(prompt, sizeof(prompt),
                 "《%s》已逾期 %zu天%zu小时。\n"
                 "按规则逾期图书无法自助归还，确定要提交吗？",
                 item->name->c_str(item->name), days, hours);
    } else {
        snprintf(prompt, sizeof(prompt), "确认归还《%s》？", item->name->c_str(item->name));
    }
    if (!tui_confirm("%s", prompt)) return false;

    borrow_result result = return_student_book(list->user, item, false);
    if (result.status == BORROW_OK) tui_info("还书成功", "%s", result.message);
    else tui_error("还书失败", "%s", result.message);
    return false;
}

void tui_student_return(student user) {
    for (;;) {
        return_context ctx = { borrow_student_books(user), user };
        if (ctx.books->size(ctx.books) == 0) {
            delete_vector(ctx.books);
            tui_info("还书", "当前没有借阅中的图书。");
            return;
        }

        static const char *const headers[] = { "ID", "书名", "作者", "期限" };
        static const int widths[] = { 8, 30, 18, 22 };

        tui_table_spec spec = {
            .title = "还书 · 我的借阅", .headers = headers, .col_widths = widths, .ncols = 4,
            .row_count = return_rows, .row_text = return_row_text, .ctx = &ctx, .page_size = 0,
            .hint = "↑↓/jk 移动  Enter 归还  q 返回", .enable_command = true,
            .on_command = run_command,
            .on_enter = return_action,
        };

        long selected = tui_table(&spec);
        delete_vector(ctx.books);
        if (selected == TUI_RESULT_COMMAND) { tui_handle_command(); continue; }
        if (selected < 0) return;
    }
}
