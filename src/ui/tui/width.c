#include "ui/tui/width.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/* 截断标记：U+2026 HORIZONTAL ELLIPSIS，显示宽度 1 */
#define UI_ELLIPSIS L'\x2026'

wchar_t *ui_wcs_from_utf8(const char *s) {
    if (!s) return NULL;

    size_t bytes = strlen(s);
    wchar_t *buf = malloc(sizeof(wchar_t) * (bytes + 1));
    if (!buf) return NULL;

    mbstate_t st;
    memset(&st, 0, sizeof(st));

    size_t out = 0;
    const char *p = s;
    while (*p) {
        wchar_t wc = 0;
        size_t k = mbrtowc(&wc, p, bytes - (size_t)(p - s), &st);
        if (k == (size_t)-1 || k == (size_t)-2) {
            /* 非法序列：按单字节降级，保证不丢内容也不死循环 */
            memset(&st, 0, sizeof(st));
            wc = (wchar_t)(unsigned char)*p;
            k = 1;
        } else if (k == 0) {
            break; /* 遇到 '\0' */
        }
        buf[out++] = wc;
        p += k;
    }
    buf[out] = L'\0';
    return buf;
}

char *ui_utf8_from_wcs(const wchar_t *s) {
    if (!s) return NULL;

    size_t cap = 4 * wcslen(s) + 1;
    char *buf = malloc(cap);
    if (!buf) return NULL;

    mbstate_t st;
    memset(&st, 0, sizeof(st));

    size_t used = 0;
    for (const wchar_t *p = s; *p; ++p) {
        char tmp[MB_LEN_MAX];
        size_t k = wcrtomb(tmp, *p, &st);
        if (k == (size_t)-1) {
            memset(&st, 0, sizeof(st));
            tmp[0] = '?';
            k = 1;
        }
        if (used + k + 1 > cap) {
            size_t ncap = (used + k) * 2 + 1;
            char *nb = realloc(buf, ncap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
            cap = ncap;
        }
        memcpy(buf + used, tmp, k);
        used += k;
    }
    buf[used] = '\0';
    return buf;
}

int ui_wcwidth(wchar_t wc) {
    int w = wcwidth(wc);
    return w < 0 ? 1 : w;
}

int ui_wcswidth(const wchar_t *s) {
    if (!s) return 0;
    int total = 0;
    for (const wchar_t *p = s; *p; ++p) {
        total += ui_wcwidth(*p);
    }
    return total;
}

int ui_utf8_width(const char *s) {
    if (!s) return 0;
    wchar_t *w = ui_wcs_from_utf8(s);
    if (!w) return 0;
    int total = ui_wcswidth(w);
    free(w);
    return total;
}

/* 按列宽把 src 复制到 dst，必要时用省略号收尾。
 * 返回实际占用的列数。 */
size_t ui_fit_wcs(const wchar_t *src, int max_cols, wchar_t *dst, size_t dstcap) {
    if (!dst || dstcap == 0) return 0;
    dst[0] = L'\0';
    if (!src || max_cols <= 0) return 0;

    size_t srclen = wcslen(src);
    int total = ui_wcswidth(src);

    if (total <= max_cols) {
        size_t n = srclen;
        if (n > dstcap - 1) n = dstcap - 1;
        wmemcpy(dst, src, n);
        dst[n] = L'\0';
        return (size_t)ui_wcswidth(dst);
    }

    /* 需要截断：预留 1 列给省略号 */
    int budget = max_cols - 1;
    int used = 0;
    size_t i = 0;
    while (src[i] && i + 2 < dstcap) {   /* i + 2 保证省略号与 '\0' 有位 */
        int cw = ui_wcwidth(src[i]);
        if (used + cw > budget) break;
        dst[i] = src[i];
        used += cw;
        ++i;
    }

    if (i + 2 <= dstcap) {
        dst[i] = UI_ELLIPSIS;
        dst[i + 1] = L'\0';
        used += 1;
    } else {
        dst[i] = L'\0';
    }
    return (size_t)used;
}

size_t ui_fit_utf8(const char *src, int max_cols, char *dst, size_t dstcap) {
    if (!dst || dstcap == 0) return 0;
    dst[0] = '\0';
    if (!src) return 0;

    wchar_t *w = ui_wcs_from_utf8(src);
    if (!w) return 0;

    size_t wcap = wcslen(w) + 2;
    wchar_t *fitted = malloc(sizeof(wchar_t) * wcap);
    if (!fitted) { free(w); return 0; }

    ui_fit_wcs(w, max_cols, fitted, wcap);

    char *u = ui_utf8_from_wcs(fitted);
    size_t cols = 0;
    if (u) {
        cols = (size_t)ui_utf8_width(u);
        ui_utf8_copy(dst, dstcap, u);
        free(u);
    }
    free(fitted);
    free(w);
    return cols;
}

size_t ui_utf8_copy(char *dst, size_t cap, const char *src) {
    if (!dst || cap == 0) return 0;
    dst[0] = '\0';
    if (!src) return 0;

    size_t used = 0;
    const char *p = src;
    while (*p) {
        unsigned char c = (unsigned char)*p;
        size_t len;
        if (c < 0x80) len = 1;
        else if ((c >> 5) == 0x6) len = 2;
        else if ((c >> 4) == 0xE) len = 3;
        else if ((c >> 3) == 0x1E) len = 4;
        else len = 1; /* 非法首字节，单字节跳过 */

        if (used + len + 1 > cap) break;
        memcpy(dst + used, p, len);
        used += len;
        p += len;
    }
    dst[used] = '\0';
    return used;
}

size_t ui_utf8_cp_count(const char *s) {
    if (!s) return 0;
    size_t n = 0;
    for (const unsigned char *p = (const unsigned char *)s; *p; ++p) {
        if ((*p & 0xC0) != 0x80) ++n; /* 非续字节即一个码点起始 */
    }
    return n;
}
