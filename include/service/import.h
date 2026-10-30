#ifndef _service_import_h
#define _service_import_h

#include <stdbool.h>
#include <stddef.h>

typedef void (*import_progress_fn)(const char *stage, size_t done,
                                   size_t total, void *context);

typedef struct {
    bool ok;
    size_t imported;
    size_t line;
    const char *message;
} import_result;

import_result import_books_file(const char *path, import_progress_fn progress,
                                void *context);
import_result import_students_file(const char *path, import_progress_fn progress,
                                   void *context);

#endif // _service_import_h