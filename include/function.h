#ifndef _function_lib
#define _function_lib

#include "uiBook.h"
#include "uiStudent.h"
#include "DataBase/DataBase.h"

void load_books_from_file(const char *filename, dataBase bookDb);
void load_students_from_file(const char *filename, dataBase studentDb);

#endif