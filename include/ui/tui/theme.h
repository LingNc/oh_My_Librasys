#ifndef _ui_theme_h
#define _ui_theme_h

/* 颜色主题：无颜色终端自动降级为 A_REVERSE / A_BOLD */

typedef enum {
    UI_PAIR_NORMAL = 0,
    UI_PAIR_TITLE,
    UI_PAIR_HILITE,
    UI_PAIR_ERROR,
    UI_PAIR_HINT,
    UI_PAIR_OK,
    UI_PAIR_COUNT
} ui_pair;

void theme_init(void);

/* 返回可直接传给 attr 参数的属性值 */
int theme_attr(ui_pair pair);

/* 是否成功启用了颜色 */
int theme_has_color(void);

#endif // _ui_theme_h
