#ifndef _function_lib
#define _function_lib

#include "uiBook.h"
#include "uiStudent.h"

uibook* load_books_from_file(const char *filename);
uistudent* load_students_from_file(const char *filename);

#endif