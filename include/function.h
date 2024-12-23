#ifndef _function_lib
#define _function_lib

#include "models/uiBook.h"
#include "models/uiStudent.h"
#include "DataBase/DataBase.h"

bool load_books_from_file(const char *filename, dataBase bookDb);
bool load_students_from_file(const char *filename, dataBase studentDb);

vector load_borrow_records(dataBase borrowDb, size_t student_id);
void save_borrow_records(dataBase borrowDb, size_t student_id, vector records);

#endif