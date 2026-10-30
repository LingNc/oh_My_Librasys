#include "ui/tui/views/main_menu.h"

#include <stdio.h>
#include <time.h>

#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/demo.h"
#include "ui/tui/theme.h"
#include "ui/tui/views/command.h"
#include "ui/tui/views/login.h"

static int draw_welcome(void *ctx, const ui_frame *f, int y) {
    (void)ctx;

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char clock_text[64];
    if (local) {
        strftime(clock_text, sizeof(clock_text), "当前时间：%Y-%m-%d %H:%M", local);
    } else {
        snprintf(clock_text, sizeof(clock_text), "当前时间：不可用");
    }

    ui_puts(y, f->x0, f->w, "欢迎使用图书管理系统", theme_attr(UI_PAIR_TITLE));
    ui_puts(y + 1, f->x0, f->w, clock_text, theme_attr(UI_PAIR_HINT));
    ui_puts(y + 2, f->x0, f->w, "请选择登录入口：", theme_attr(UI_PAIR_NORMAL));
    return y + 4;
}

int tui_main_menu(void) {
    static const char *const items[] = {
        "1. 学生入口",
        "2. 管理员入口",
        "3. 组件演示",
        "4. 退出",
    };

    tui_menu_spec spec = {
        .title = "图书管理系统",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
        .pre_info = draw_welcome,
        .hint = "↑↓/jk 选择  Enter 确认  q 退出  / 命令",
        .enable_command = true,
    };

    for (;;) {
        int choice = tui_menu(&spec);
        if (choice == TUI_RESULT_COMMAND) { tui_handle_command(); continue; }
        if (choice < 0 || choice == 3) return 0;

        switch (choice) {
        case 0:
            tui_student_login();
            break;
        case 1:
            tui_manager_login();
            break;
        case 2:
            tui_demo();
            break;
        default:
            tui_error("错误", "未知菜单项");
            break;
        }
    }
}