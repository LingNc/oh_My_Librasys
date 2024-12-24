#ifndef _page_h
#define _page_h

#include "Tools/Vector.h"
#include "DataBase/DataBase.h"

#define DEFAULT_PAGE_SIZE 10

void display_page(vector content, int page, int total_pages, int highlight, void *args[]);
int handle_page_input(int *page, int total_pages, int *highlight, int *choice, void **args);
void page(dataBase db, int pageSize, void (**funcs)(void *), void **arg);

#endif // _page_h