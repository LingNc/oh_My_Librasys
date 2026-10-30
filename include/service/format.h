#ifndef _service_format_h
#define _service_format_h

#include <stddef.h>

#include "models/Book.h"
#include "models/Manager.h"
#include "models/Student.h"

void format_book_row(book item, char *out, size_t cap);
void format_student_row(student item, char *out, size_t cap);
void format_manager_row(manager item, char *out, size_t cap);

#endif // _service_format_h