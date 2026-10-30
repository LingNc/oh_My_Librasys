#ifndef _service_admin_h
#define _service_admin_h

#include <stddef.h>

#include "models/Book.h"
#include "models/Manager.h"
#include "models/Student.h"
#include "Tools/Vector.h"

typedef enum {
    ADMIN_OP_OK = 0,
    ADMIN_OP_INVALID,
    ADMIN_OP_DUPLICATE,
    ADMIN_OP_NOT_FOUND,
    ADMIN_OP_BUSY,
    ADMIN_OP_PASSWORD_MISMATCH,
    ADMIN_OP_IO_ERROR
} admin_op_status;

typedef struct {
    admin_op_status status;
    const char *message;
} admin_op_result;

/* 返回独立快照 vector，调用方必须 delete_vector() */
vector admin_list_books(void);
vector admin_list_students(void);
vector admin_list_managers(void);

admin_op_result admin_add_book(const char *isbn, const char *name,
                               const char *author, const char *publisher,
                               const char *published_at);
admin_op_result admin_update_book(book item, const char *isbn, const char *name,
                                  const char *author, const char *publisher,
                                  const char *published_at);
admin_op_result admin_delete_book(book item);

admin_op_result admin_add_student(size_t student_id, const char *name,
                                  const char *class_name, const char *department);
admin_op_result admin_update_student(student item, const char *name,
                                     const char *class_name, const char *department);
admin_op_result admin_delete_student(student item);

admin_op_result admin_add_manager(manager current, size_t id, const char *name,
                                  const char *password);
admin_op_result admin_update_manager(manager current, manager item, const char *name);
admin_op_result admin_delete_manager(manager current, manager item);
admin_op_result admin_change_password(manager current, manager item,
                                      const char *password, const char *confirmation);

#endif // _service_admin_h