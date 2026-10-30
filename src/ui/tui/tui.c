/*
 * TUI 骨架（M0）
 *
 * 当前阶段只负责：
 *   - 正确的初始化/退出顺序（setlocale 必须在 initscr 之前）
 *   - 中文宽字符显示与对齐验证
 *   - 异常退出时恢复终端
 *
 * 后续里程碑在此基础上加入 window / key / components，不要在这里堆业务逻辑。
 */

#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>

#include "ui/tui/tui.h"
#include "ui/tui/demo.h"
#include "ui/tui/theme.h"

static bool g_tui_active = false;

static void tui_atexit(void) {
    tui_shutdown();
}

static void tui_on_signal(int sig) {
    tui_shutdown();
    signal(sig, SIG_DFL);
    raise(sig);
}

bool tui_init(void) {
    if (g_tui_active) return true;

    /* 非交互终端（管道、重定向、cron）直接拒绝，
     * 否则 ncurses 会把界面写进日志，且读键永远 EOF。 */
    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
        fprintf(stderr, "TUI 需要交互式终端（stdin/stdout 必须是 tty）\n");
        return false;
    }

    /* 必须在 initscr 之前，否则宽字符无法正确显示 */
    setlocale(LC_ALL, "");

    if (!initscr()) {
        fprintf(stderr, "无法初始化终端界面（TERM=%s）\n",
                getenv("TERM") ? getenv("TERM") : "(unset)");
        return false;
    }

    cbreak();              /* 逐键响应，保留信号 */
    noecho();
    keypad(stdscr, TRUE);  /* 方向键/功能键 */
    curs_set(0);
    set_escdelay(25);      /* 降低 ESC 判定延迟（ncurses >= 6.1） */

    if (has_colors()) {
        start_color();
        use_default_colors();
    }
    theme_init();

    g_tui_active = true;
    atexit(tui_atexit);
    signal(SIGINT, tui_on_signal);
    signal(SIGTERM, tui_on_signal);
    signal(SIGHUP, tui_on_signal);

    return true;
}

void tui_shutdown(void) {
    if (!g_tui_active) return;
    g_tui_active = false;
    if (!isendwin()) {
        endwin();
    }
}

int tui_run(void) {
    if (!tui_init()) {
        return 1;
    }

    /* 非 UTF-8 终端直接回退，避免满屏乱码 */
    if (MB_CUR_MAX <= 1) {
        tui_shutdown();
        fprintf(stderr, "当前 locale 不是多字节编码，无法显示中文，回退到 CLI\n");
        return 1;
    }

    int rc = tui_demo();

    tui_shutdown();
    return rc;
}
