#ifndef _page_h
#define _page_h

#include "Tools/Vector.h"
#include "DataBase/DataBase.h"

void display_page(vector content, int highlight, const char prefix[]);
bool handle_page_input(int *page, int total_pages, int *highlight, int *choice, void **args);
void page(dataBase db, int page_size, const char *prefix);

#endif // _page_h