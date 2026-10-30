#ifndef _ui_width_h
#define _ui_width_h

#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

/*
 * 宽字符 / UTF-8 工具（不依赖 ncurses，可独立单元测试）
 *
 * 所有"显示列宽"一律用 ui_wcwidth / ui_wcswidth 计算，
 * 中文占 2 列，禁止用 strlen 做对齐。
 */

/* UTF-8 <-> 宽字符；返回 malloc 分配的缓冲区，调用方 free；NULL 安全 */
wchar_t *ui_wcs_from_utf8(const char *s);
char    *ui_utf8_from_wcs(const wchar_t *s);

/* 单个宽字符的显示宽度；不可打印/无法确定时按 1 列处理 */
int ui_wcwidth(wchar_t wc);

/* 宽字符串/UTF-8 字符串的显示宽度（列） */
int ui_wcswidth(const wchar_t *s);
int ui_utf8_width(const char *s);

/* 按"列宽"截断：
 *   截断时在末尾追加 '…'（宽度 1）
 *   返回实际占用的列数
 * dstcap 是 dst 的元素个数（宽字符为 wchar_t 个数，UTF-8 为字节数）
 */
size_t ui_fit_wcs(const wchar_t *src, int max_cols, wchar_t *dst, size_t dstcap);
size_t ui_fit_utf8(const char *src, int max_cols, char *dst, size_t dstcap);

/* 按完整码点复制 UTF-8（不会截断出半个字符），返回写入字节数 */
size_t ui_utf8_copy(char *dst, size_t cap, const char *src);

/* UTF-8 字符串的码点个数 */
size_t ui_utf8_cp_count(const char *s);

#endif // _ui_width_h
