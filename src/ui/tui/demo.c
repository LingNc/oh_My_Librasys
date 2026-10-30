#include "ui/tui/demo.h"

#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "ui/tui/components/dialog.h"
#include "ui/tui/components/form.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/components/table.h"
#include "ui/tui/key.h"
#include "ui/tui/theme.h"
#include "ui/tui/window.h"
#include "ui/tui/width.h"

/* ---------------------------------------------------------------- 宽度自检 */

static int demo_width_info(void *ctx, const ui_frame *f, int y) {
    (void)ctx;

    static const char *const content[] = {
        "书名：《C程序设计》",
        "作者：谭浩强",
        "ISBN：978-7-302-12345-6",
        "Mixed 中英文 text 混合",
        "最宽的一行：简体中文与 ASCII",
    };
    const int n = (int)(sizeof(content) / sizeof(content[0]));

    int maxw = 0;
    for (int i = 0; i < n; ++i) {
        int w = ui_utf8_width(content[i]);
        if (w > maxw) maxw = w;
    }

    int boxw = maxw + 4;          /* 左右各留 1 列内边距 + 两侧边框 */
    if (boxw > f->w) boxw = f->w;

    ui_puts(y, f->x0, 2, "┌", A_NORMAL);
    ui_hline_w(y, f->x0 + 1, boxw - 2, L'─');
    ui_puts(y, f->x0 + boxw - 1, 2, "┐", A_NORMAL);
    ++y;

    for (int i = 0; i < n && y < f->y1 - 1; ++i, ++y) {
        ui_puts(y, f->x0, 2, "│", A_NORMAL);
        ui_puts_center(y, f->x0 + 1, boxw - 2, content[i], A_NORMAL);
        ui_puts(y, f->x0 + boxw - 1, 2, "│", A_NORMAL);
    }

    ui_puts(y, f->x0, 2, "└", A_NORMAL);
    ui_hline_w(y, f->x0 + 1, boxw - 2, L'─');
    ui_puts(y, f->x0 + boxw - 1, 2, "┘", A_NORMAL);
    ++y;

    y += 1;
    char buf[128];
    snprintf(buf, sizeof(buf), "内容最大列宽 = %d，框宽 = %d（边框应正好贴合）", maxw, boxw);
    ui_puts(y, f->x0, f->w, buf, theme_attr(UI_PAIR_HINT));
    ++y;

    snprintf(buf, sizeof(buf), "当前 locale = %s", setlocale(LC_ALL, NULL));
    ui_puts(y, f->x0, f->w, buf, theme_attr(UI_PAIR_HINT));
    return y + 2;
}

/* ---------------------------------------------------------------- 表格演示 */

#define DEMO_ROWS 42

static void demo_row_text(void *ctx, size_t row, size_t col, char *out, size_t cap) {
    (void)ctx;
    static const char *const authors[] = {"谭浩强", "K&R", "Knuth", "Bryant", "Wirth"};
    static const char *const states[] = {"可借", "已借出", "已借出（逾期）"};

    switch (col) {
    case 0:
        snprintf(out, cap, "%zu", row + 1);
        break;
    case 1:
        snprintf(out, cap, "《C程序设计 第%zu版》", row % 7 + 1);
        break;
    case 2:
        snprintf(out, cap, "%s", authors[row % 5]);
        break;
    default:
        snprintf(out, cap, "%s", states[row % 3]);
        break;
    }
}

static size_t demo_row_count(void *ctx) {
    (void)ctx;
    return DEMO_ROWS;
}

static void demo_table(void) {
    static const char *const headers[] = {"序号", "书名", "作者", "状态"};
    static const int widths[] = {6, 30, 12, 18};

    tui_table_spec spec = {
        .title = "组件演示 · 表格分页",
        .headers = headers,
        .col_widths = widths,
        .ncols = 4,
        .row_count = demo_row_count,
        .row_text = demo_row_text,
        .ctx = NULL,
        .page_size = 10,
    };

    for (;;) {
        long row = tui_table(&spec);
        if (row < 0) return;
        tui_info("选中行", "你选中了第 %ld 行\n（真实业务中这里会打开详情页）", row + 1);
    }
}

/* ---------------------------------------------------------------- 表单演示 */

static const char *validate_required(const char *v) {
    return (v && v[0]) ? NULL : "不能为空";
}

static void demo_form(void) {
    char id[32] = "";
    char name[64] = "";
    char passwd[64] = "";

    tui_field fields[] = {
        {"学号", id, sizeof(id), TUI_FIELD_NUMERIC, validate_required},
        {"姓名", name, sizeof(name), 0, validate_required},
        {"密码", passwd, sizeof(passwd), TUI_FIELD_MASKED, validate_required},
    };

    tui_form_result r = tui_form("组件演示 · 表单", fields,
                                 sizeof(fields) / sizeof(fields[0]),
                                 "Tab/↑↓ 切换  退格删除  Enter 提交  Esc 取消");

    if (r == TUI_FORM_OK) {
        tui_info("提交结果", "学号: %s\n姓名: %s\n密码长度: %zu",
                 id, name, strlen(passwd));
    }
}

/* ---------------------------------------------------------------- 菜单演示 */

static int demo_menu_pre(void *ctx, const ui_frame *f, int y) {
    (void)ctx;
    ui_puts(y, f->x0, f->w, "这是一个带 pre_info 的菜单：", theme_attr(UI_PAIR_HINT));
    ui_puts(y + 1, f->x0, f->w, "选中条应整行反色，中文不错位。", theme_attr(UI_PAIR_HINT));
    ui_puts(y + 2, f->x0, f->w, "按数字键可直接跳转，Enter 确认，q 返回。",
            theme_attr(UI_PAIR_HINT));
    return y + 4;
}

static void demo_menu(void) {
    static const char *const items[] = {
        "1. 查看图书列表",
        "2. 增加图书",
        "3. 删除图书",
        "4. 借阅统计",
        "5. 返回上级",
    };

    tui_menu_spec spec = {
        .title = "组件演示 · 菜单",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
        .pre_info = demo_menu_pre,
        .post_info = NULL,
        .ctx = NULL,
    };

    for (;;) {
        int idx = tui_menu(&spec);
        if (idx < 0 || idx == 4) return;
        tui_toast("选择了：%s", items[idx]);
    }
}

/* ---------------------------------------------------------------- 对话框演示 */

static void demo_dialog(void) {
    static const char *const items[] = {
        "1. 提示框",
        "2. 错误框",
        "3. 确认框",
        "4. 自动消失提示 (toast)",
        "5. 返回",
    };

    tui_menu_spec spec = {
        .title = "组件演示 · 对话框",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
    };

    for (;;) {
        int idx = tui_menu(&spec);
        if (idx < 0 || idx == 4) return;
        switch (idx) {
        case 0:
            tui_info("提示", "这是一条提示信息。\n中文与 English 混排应当自动折行且不劈开全角字符。");
            break;
        case 1:
            tui_error("删除失败", "该图书仍存在未归还的借阅记录，无法删除。");
            break;
        case 2:
            if (tui_confirm("确定要删除《C程序设计》吗？此操作不可撤销。")) {
                tui_toast("已删除（演示）");
            } else {
                tui_toast("已取消");
            }
            break;
        default:
            tui_toast("操作成功：%d 条记录已保存", 128);
            break;
        }
    }
}

/* ---------------------------------------------------------------- 入口 */

static int demo_width_page(void) {
    tui_menu_spec spec = {
        .title = "组件演示 · 宽度自检",
        .items = (const char *const[]){ "1. 返回" },
        .count = 1,
        .initial = 0,
        .pre_info = demo_width_info,
    };
    tui_menu(&spec);
    return -1;
}

int tui_demo(void) {
    static const char *const items[] = {
        "1. 宽度自检（中文对齐）",
        "2. 菜单与高亮",
        "3. 表格分页",
        "4. 表单输入",
        "5. 对话框",
        "6. 退出",
    };

    tui_menu_spec spec = {
        .title = "图书管理系统 · TUI 组件演示",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
        .hint = "↑↓/jk 选择  Enter 确认  q 退出",
    };

    for (;;) {
        int idx = tui_menu(&spec);
        if (idx < 0 || idx == 5) break;
        switch (idx) {
        case 0: demo_width_page(); break;
        case 1: demo_menu();       break;
        case 2: demo_table();      break;
        case 3: demo_form();       break;
        case 4: demo_dialog();     break;
        default: break;
        }
    }
    return 0;
}
