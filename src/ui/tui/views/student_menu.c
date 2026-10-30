#include "ui/tui/views/student_menu.h"

#include <stdio.h>

#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/theme.h"

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
    };

    for (;;) {
        int choice = tui_menu(&spec);
        if (choice < 0 || choice == 2) return;
        tui_info("功能迁移中", "该功能将在 M4 学生功能阶段接入。");
    }
}