#include "ui/tui/views/admin_menu.h"

#include <stdio.h>

#include "DataBase/DataBase.h"
#include "ui/tui/components/dialog.h"
#include "ui/tui/components/menu.h"
#include "ui/tui/theme.h"
#include "ui/tui/views/admin_crud.h"
#include "ui/tui/views/command.h"

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

static void run_command(void *ctx) {
    (void)ctx;
    tui_handle_command();
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
        "8. 批量导入学生",
        "9. 批量导入图书",
        "10. 退出登录",
    };

    tui_menu_spec spec = {
        .title = "管理员控制台",
        .items = items,
        .count = (int)(sizeof(items) / sizeof(items[0])),
        .initial = 0,
        .pre_info = draw_admin_info,
        .ctx = user,
        .hint = "↑↓/jk 选择  Enter 确认  q 返回  / 命令",
        .enable_command = true,
        .on_command = run_command,
    };

    for (;;) {
        int choice = tui_menu(&spec);
        if (choice == TUI_RESULT_COMMAND) { tui_handle_command(); continue; }
        if (choice < 0 || choice == 9) return;
        switch (choice) {
        case 0: tui_admin_students(user); break;
        case 1: tui_admin_add_student(user); break;
        case 2: tui_admin_books(user); break;
        case 3: tui_admin_add_book(user); break;
        case 4: tui_admin_managers(user); break;
        case 5: tui_admin_add_manager(user); break;
        case 6: tui_admin_change_password(user); break;
        case 7: tui_admin_import_students(user); break;
        case 8: tui_admin_import_books(user); break;
        default: tui_error("错误", "未知菜单项"); break;
        }
    }
}