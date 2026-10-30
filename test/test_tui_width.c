#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "ui/tui/width.h"

static void test_ascii_width(void) {
    assert(ui_utf8_width("hello") == 5);
    assert(ui_utf8_width("") == 0);
    assert(ui_utf8_width(NULL) == 0);
}

static void test_chinese_width(void) {
    /* 每个中日韩字符占 2 列 */
    assert(ui_utf8_width("图书") == 4);
    assert(ui_utf8_width("图书 A") == 6);   /* 4 + 1 + 1 */
    /* 《(2) + C(1) + 程序设计(4 字 = 8 列) + 》(2) = 13 */
    assert(ui_utf8_width("《C程序设计》") == 2 + 1 + 2 * 4 + 2);
}

static void test_round_trip(void) {
    const char *src = "图书管理 system 123";
    wchar_t *w = ui_wcs_from_utf8(src);
    assert(w != NULL);
    char *back = ui_utf8_from_wcs(w);
    assert(back != NULL);
    assert(strcmp(src, back) == 0);
    free(w);
    free(back);
}

static void test_utf8_cp_count(void) {
    assert(ui_utf8_cp_count("abc") == 3);
    assert(ui_utf8_cp_count("图书") == 2);
    assert(ui_utf8_cp_count("") == 0);
    assert(ui_utf8_cp_count(NULL) == 0);
}

static void test_fit_no_truncate(void) {
    wchar_t out[32];
    size_t used = ui_fit_wcs(L"图书", 10, out, 32);
    assert(used == 4);
    assert(wcscmp(out, L"图书") == 0);

    char uout[32];
    used = ui_fit_utf8("图书", 10, uout, sizeof(uout));
    assert(used == 4);
    assert(strcmp(uout, "图书") == 0);
}

static void test_fit_truncate_never_splits_wide_char(void) {
    wchar_t out[32];

    /* 只有 3 列：2 列中文 + 省略号 */
    size_t used = ui_fit_wcs(L"图书管理", 3, out, 32);
    assert(used <= 3);
    assert(wcslen(out) >= 1);
    assert(out[wcslen(out) - 1] == L'\x2026');

    /* 只有 1 列：放不下中文，只剩省略号 */
    used = ui_fit_wcs(L"图书", 1, out, 32);
    assert(used <= 1);
    assert(ui_wcswidth(out) <= 1);

    /* 结果宽度绝不能超过限制 */
    for (int cols = 1; cols <= 6; ++cols) {
        ui_fit_wcs(L"图书管理abc", cols, out, 32);
        assert(ui_wcswidth(out) <= cols);
    }

    /* UTF-8 版本同样保证完整码点 */
    char uout[64];
    for (int cols = 1; cols <= 6; ++cols) {
        ui_fit_utf8("图书管理abc", cols, uout, sizeof(uout));
        assert(ui_utf8_width(uout) <= cols);
    }
}

static void test_utf8_copy_never_splits(void) {
    char out[8];
    /* "图书" 占 6 字节，cap=7 可完整放下 */
    size_t n = ui_utf8_copy(out, 7, "图书");
    assert(n == 6);
    assert(strcmp(out, "图书") == 0);

    /* cap=5 放不下第二个汉字，只能复制第一个 */
    n = ui_utf8_copy(out, 5, "图书");
    assert(n == 3);
    assert(strcmp(out, "图") == 0);
}

int main(void) {
    /* 宽度计算依赖 locale，必须设置成 UTF-8 */
    if (!setlocale(LC_ALL, "C.UTF-8") && !setlocale(LC_ALL, "en_US.UTF-8")
        && !setlocale(LC_ALL, "")) {
        fprintf(stderr, "无法设置 locale\n");
        return 1;
    }

    test_ascii_width();
    test_chinese_width();
    test_round_trip();
    test_utf8_cp_count();
    test_fit_no_truncate();
    test_fit_truncate_never_splits_wide_char();
    test_utf8_copy_never_splits();

    printf("All tui width tests passed!\n");
    return 0;
}
