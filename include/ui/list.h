
#ifndef _list_h
#define _list_h

#define MAX_INPUT 100

#include <wchar.h>
#include <stdbool.h>
#include "Tools/String.h"
#include "Tools/Vector.h"

// 按照 wchar_t 字符串数组显示每一行内容
void display_wchar(int highlight,const wchar_t **lines,int n_lines);
// 按照 type 类型显示结构体为一行
void display_one_line(bool isHighlight,string type,void *data);
// 按照 vector 显示vector中所有内容
void display(int highlight,vector lines);

#endif // _list_h