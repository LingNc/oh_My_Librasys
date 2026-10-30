#include "ui/tui/views/main_menu.h"

#include <stdio.h>
#include <time.h>

#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/demo.h"
#include "ui/tui/theme.h"
#include "ui/tui/views/command.h"
#include "ui/tui/views/login.h"

/*
 * ONMYLIB 机械风格标识（纯 ASCII）
 * 每行 41 列，整体宽 41。
 */
static const char *const logo[] = {
    " ###  #   # #   # #   # #     ##### #### ",
    "#   # ##  # ## ##  # #  #       #   #   #",
    "#   # # # # # # #   #   #       #   #### ",
    "#   # #  ## #   #   #   #       #   #   #",
    " ###  #   # #   #   #   ##### ##### #### ",
};

#define LOGO_LINES ((int)(sizeof(logo) / sizeof(logo[0])))
#define LOGO_WIDTH 41

/* 把标识行画到内容区，水平居中；返回下一个可用行 */
static int draw_logo(const ui_frame *f, int y) {
    if (f->w < LOGO_WIDTH + 4 || f->y1 - y < LOGO_LINES + 2) {
        ui_puts(y, f->x0, f->w, "ONMYLIB // LIBRARY SYSTEM",
                theme_attr(UI_PAIR_ART));
        return y + 2;
    }

    for (int i = 0; i < LOGO_LINES; ++i) {
        ui_puts_center(y + i, f->x0, f->w, logo[i], theme_attr(UI_PAIR_ART));
    }
    y += LOGO_LINES;

    ui_puts_center(y, f->x0, f->w, "[ L I B R A R Y   M A N A G E M E N T   S Y S T E M ]",
                   theme_attr(UI_PAIR_HINT));
    return y + 2;
}

static int draw_welcome(void *ctx, const ui_frame *f, int y) {
    (void)ctx;

    y = draw_logo(f, y);

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char clock_text[96];
    if (local) {
        strftime(clock_text, sizeof(clock_text), "SYS.TIME  %Y-%m-%d %H:%M", local);
    } else {
        snprintf(clock_text, sizeof(clock_text), "SYS.TIME  N/A");
    }

    ui_puts(y, f->x0, f->w, "----------------------------------------",
            theme_attr(UI_PAIR_BORDER));
    ui_puts(y + 1, f->x0, f->w, clock_text, theme_attr(UI_PAIR_HINT));
    ui_puts(y + 2, f->x0, f->w, "SELECT LOGIN CHANNEL", theme_attr(UI_PAIR_HINT));
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
        .title = "ONMYLIB // LIBRARY SYSTEM",
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