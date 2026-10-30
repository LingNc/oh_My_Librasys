#include "ui/tui/views/main_menu.h"

#include <stdio.h>
#include <time.h>

#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/theme.h"
#include "ui/tui/views/command.h"
#include "ui/tui/views/login.h"

/*
 * ONMYLIB 斜体大字标识（纯 ASCII，user-provided slant style）
 * 6 行，每行 87 列。
 */
static const char *const logo[] = {
    "                __         __  ___         __    _ __               _____              ",
    "         ____  / /_       /  |/  /_  __   / /   (_) /_  _________ _/ ___/__  _______   ",
    "        / __ \\/ __ \\     / /|_/ / / / /  / /   / / __ \\/ ___/ __ `/\\__ \\/ / / / ___/   ",
    "       / /_/ / / / /    / /  / / /_/ /  / /___/ / /_/ / /  / /_/ /___/ / /_/ (__  )    ",
    "       \\____/_/ /_/    /_/  /_/\\__, /  /_____/_/_.___/_/   \\__,_//____/\\__, /____/     ",
    "                              /____/                                  /____/           ",
};

#define LOGO_LINES ((int)(sizeof(logo) / sizeof(logo[0])))
#define LOGO_WIDTH 87

/* 画标识；窄屏或高度不足时回退为单行文字。返回下一个可用行 */
static int draw_logo(const ui_frame *f, int y) {
    bool fits_width = f->w >= LOGO_WIDTH;
    bool fits_height = f->y1 - y >= LOGO_LINES + 6;

    if (!fits_width || !fits_height) {
        ui_puts_center(y, f->x0, f->w, "O N M Y L I B", theme_attr(UI_PAIR_ART));
        return y + 2;
    }

    for (int i = 0; i < LOGO_LINES; ++i) {
        ui_puts_center(y + i, f->x0, f->w, logo[i], theme_attr(UI_PAIR_ART));
    }
    y += LOGO_LINES;

    ui_puts_center(y, f->x0, f->w,
                   "[ L I B R A R Y   M A N A G E M E N T   S Y S T E M ]",
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

    ui_puts_center(y, f->x0, f->w, "==============================",
                   theme_attr(UI_PAIR_BORDER));
    ui_puts_center(y + 1, f->x0, f->w, clock_text, theme_attr(UI_PAIR_HINT));
    ui_puts_center(y + 2, f->x0, f->w, "SELECT LOGIN CHANNEL", theme_attr(UI_PAIR_HINT));
    return y + 4;
}

static void show_about(void) {
    tui_info("ABOUT / 关于",
             "ONMYLIB // LIBRARY MANAGEMENT SYSTEM\n"
             "\n"
             "一个用 C 语言从零实现的图书管理系统，不依赖第三方容器库：\n"
             "自研泛型 Vector / String / Pair / Hash / Trie，\n"
             "基于本地文件 + 哈希索引的存储层，\n"
             "并提供命令行与 ncursesw 终端两套界面。\n"
             "\n"
             "主要作者    Leabol    LingNc\n"
             "\n"
             "版本        TUI 1.0\n"
             "终端界面    ncursesw（宽字符 / 灰阶主题）\n"
             "构建        make  |  make UI=cli\n"
             "启动        ./main  |  ./main --tui\n"
             "\n"
             "快捷键      / 命令面板    Tab 补全    Enter 确认    q/Esc 返回");
}

static void run_command(void *ctx) {
    (void)ctx;
    tui_handle_command();
}

int tui_main_menu(void) {
    static const char *const items[] = {
        "1. 学生入口",
        "2. 管理员入口",
        "3. 关于",
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
        .on_command = run_command,
        .center = true,
        .vcenter = true,
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
            show_about();
            break;
        default:
            tui_error("错误", "未知菜单项");
            break;
        }
    }
}