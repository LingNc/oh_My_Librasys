#include "ui/tui/views/student_menu.h"

#include <stdio.h>

#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/theme.h"
#include "ui/tui/views/student_borrow.h"
#include "ui/tui/views/command.h"

static void run_command(void *ctx) {
    (void)ctx;
    tui_handle_command();
}

static int draw_student_info(void *ctx, const ui_frame *f, int y) {
    student user = ctx;
    char info[512];
    snprintf(info, sizeof(info), "学生：%s  学号：%zu",
             user->name->c_str(user->name), user->studentID);
    ui_puts(y, f->x0, f->w, info, theme_attr(UI_PAIR_TITLE));

    snprintf(info, sizeof(info), "班级：%s  学院：%s  已借：%d/%d",
             user->class->c_str(user->class),
             user->department->c_str(user->department),
             user->borrowedCount, MAX_AVAILABLE);
    ui_puts(y + 1, f->x0, f->w, info, theme_attr(UI_PAIR_HINT));
    return y + 3;
}

void tui_student_menu(student user) {
    static const char *const items[] = {
        "1. 借书",
        "2. 还书",
        "3. 退出登录",
    };

    tui_menu_spec spec = {
        .title = "学生控制台",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
        .pre_info = draw_student_info,
        .ctx = user,
        .hint = "↑↓/jk 选择  Enter 确认  q 返回  / 命令",
        .enable_command = true,
        .on_command = run_command,
    };

    for (;;) {
        int choice = tui_menu(&spec);
        if (choice == TUI_RESULT_COMMAND) { tui_handle_command(); continue; }
        if (choice < 0 || choice == 2) return;
        switch (choice) {
        case 0: tui_student_borrow(user); break;
        case 1: tui_student_return(user); break;
        default: tui_error("错误", "未知菜单项"); break;
        }
    }
}