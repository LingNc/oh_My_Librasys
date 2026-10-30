#ifndef _ui_theme_h
#define _ui_theme_h

/*
 * 冷调机械复古主题：
 *   - 有 256 色时使用灰阶（232..255），整体为金属灰
 *   - 只有 8/16 色时退化为白/黑 + A_DIM / A_REVERSE
 *   - 无颜色终端退化为 A_BOLD / A_REVERSE / A_DIM
 */

typedef enum {
    UI_PAIR_NORMAL = 0,
    UI_PAIR_TITLE,     /* 标题栏：亮灰底 + 深灰字 */
    UI_PAIR_HILITE,    /* 选中行：深字 + 亮灰底 */
    UI_PAIR_ERROR,     /* 异常：亮字 + 深灰底（不用暖色） */
    UI_PAIR_HINT,      /* 次要信息：中灰 */
    UI_PAIR_OK,        /* 成功：亮灰 */
    UI_PAIR_ART,       /* ASCII 标识 */
    UI_PAIR_BORDER,    /* 外框 */
    UI_PAIR_COUNT
} ui_pair;

void theme_init(void);

/* 返回可直接传给 attr 参数的属性值 */
int theme_attr(ui_pair pair);

/* 是否成功启用了颜色 */
int theme_has_color(void);

/* 是否启用了 256 色灰阶 */
int theme_has_gray_ramp(void);

#endif // _ui_theme_h
