#include "ui/tui/views/login.h"

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include "service/auth.h"
#include "ui/tui/components/dialog.h"
#include "ui/tui/components/form.h"
#include "ui/tui/views/admin_menu.h"
#include "ui/tui/views/student_menu.h"

static const char *validate_id(const char *value) {
    if (!value || !value[0]) return "不能为空";

    errno = 0;
    char *end = NULL;
    uintmax_t parsed = strtoumax(value, &end, 10);
    if (errno != 0 || !end || *end != '\0' || parsed > SIZE_MAX) {
        return "请输入有效的正整数";
    }
    return parsed == 0 ? "必须大于 0" : NULL;
}

static const char *validate_password(const char *value) {
    return value && value[0] ? NULL : "不能为空";
}

void tui_manager_login(void) {
    for (int attempts = 0; attempts < 3; ++attempts) {
        char id_text[32] = "";
        char password[128] = "";
        tui_field fields[] = {
            { "管理员 ID", id_text, sizeof(id_text), TUI_FIELD_NUMERIC, validate_id },
            { "密码", password, sizeof(password), TUI_FIELD_MASKED, validate_password },
        };

        if (tui_form("管理员登录", fields,
                     sizeof(fields) / sizeof(fields[0]),
                     "Tab/↑↓ 切换  Enter 登录  Esc 返回") != TUI_FORM_OK) {
            return;
        }

        size_t id = (size_t)strtoumax(id_text, NULL, 10);
        manager_auth_result result = auth_login_manager(id, password);
        switch (result.status) {
        case AUTH_OK:
            tui_admin_menu(result.user);
            return;
        case AUTH_USER_NOT_FOUND:
            tui_error("登录失败", "管理员 ID %zu 不存在", id);
            break;
        case AUTH_PASSWORD_NOT_SET:
            tui_error("登录失败", "该管理员尚未设置密码");
            return;
        case AUTH_BAD_PASSWORD:
            tui_error("登录失败", "密码错误，还可尝试 %d 次", 2 - attempts);
            break;
        }
    }
    tui_error("登录失败", "密码连续错误 3 次，已返回主菜单");
}

void tui_student_login(void) {
    char id_text[32] = "";
    tui_field fields[] = {
        { "学号", id_text, sizeof(id_text), TUI_FIELD_NUMERIC, validate_id },
    };

    if (tui_form("学生登录", fields, 1,
                 "Enter 登录  Esc 返回") != TUI_FORM_OK) {
        return;
    }

    size_t student_id = (size_t)strtoumax(id_text, NULL, 10);
    student user = auth_find_student(student_id);
    if (!user) {
        tui_error("登录失败", "学号 %zu 不存在", student_id);
        return;
    }
    tui_student_menu(user);
}