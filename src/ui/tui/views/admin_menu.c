#include "ui/tui/views/admin_menu.h"

#include <stdio.h>

#include "DataBase/DataBase.h"
#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/theme.h"

extern dataBase managerDb;

static int draw_admin_info(void *ctx, const ui_frame *f, int y) {
    manager user = ctx;
    char info[512];
    snprintf(info, sizeof(info), "当前账号：ID %zu  姓名 %s",
             user->id, user->name->c_str(user->name));
    ui_puts(y, f->x0, f->w, info, theme_attr(UI_PAIR_TITLE));

    snprintf(info, sizeof(info), "注册日期：%s  注册人：%s  管理员总数：%zu",
             user->registration_date->c_str(user->registration_date),
             user->registered_by->c_str(user->registered_by),
             managerDb->size(managerDb));
    ui_puts(y + 1, f->x0, f->w, info, theme_attr(UI_PAIR_HINT));
    return y + 3;
}

void tui_admin_menu(manager user) {
    static const char *const items[] = {
        "1. 学生管理",
        "2. 新建学生",
        "3. 图书管理",
        "4. 增加图书",
        "5. 管理员管理",
        "6. 增加管理员",
        "7. 修改密码",
        "8. 退出登录",
    };

    tui_menu_spec spec = {
        .title = "管理员控制台",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
        .pre_info = draw_admin_info,
        .ctx = user,
    };

    for (;;) {
        int choice = tui_menu(&spec);
        if (choice < 0 || choice == 7) return;
        tui_info("功能迁移中", "该功能将在 M3 管理员功能阶段接入。");
    }
}