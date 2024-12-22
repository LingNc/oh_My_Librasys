#ifndef _page_h
#define _page_h

#include "Tools/Vector.h"
#include "DataBase/DataBase.h"

void display_page(vector content, int page, int total_pages, int highlight, const char *prefix, void *args[]);
bool handle_page_input(int *page, int total_pages, int *highlight, int *choice, void **args);
void page(dataBase db, int pageSize, const char *prefix, void (*func)(void *));

#endif // _page_h