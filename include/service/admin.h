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

/* 键列表：只保存存在的记录键，配合按需加载避免一次性构造全部对象 */
typedef struct {
    size_t *keys;
    size_t count;
} admin_keys;

admin_keys admin_book_keys(void);
admin_keys admin_student_keys(void);
admin_keys admin_manager_keys(void);
void admin_keys_free(admin_keys *keys);

book admin_get_book(size_t key);
student admin_get_student(size_t key);
manager admin_get_manager(size_t key);

admin_op_result admin_add_book(const char *isbn, const char *name,
                               const char *author, const char *publisher,
                               const char *published_at);
admin_op_result admin_update_book(book item, const char *isbn, const char *name,
                                  const char *author, const char *publisher,
                                  const char *published_at);
admin_op_result admin_delete_book(book item);
student admin_book_borrower(book item);
admin_op_result admin_force_return(book item);

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