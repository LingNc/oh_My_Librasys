#ifndef _service_borrow_h
#define _service_borrow_h

#include <stdbool.h>
#include <stddef.h>

#include "models/Book.h"
#include "models/Student.h"
#include "Tools/Vector.h"

typedef enum {
    BORROW_OK = 0,
    BORROW_BOOK_NOT_FOUND,
    BORROW_STUDENT_NOT_FOUND,
    BORROW_ALREADY_BORROWED,
    BORROW_MAX_REACHED,
    BORROW_RECORD_NOT_FOUND,
    BORROW_OVERDUE
} borrow_status;

typedef struct {
    borrow_status status;
    const char *message;
} borrow_result;

/* 借书：图书必须可借，且学生未达最大借阅数量 */
borrow_result borrow_student_book(student user, book item);

/* 还书：allow_overdue 为 false 时逾期书拒绝归还（学生自助），
 * 为 true 时允许（管理员强制/代办） */
borrow_result return_student_book(student user, book item, bool allow_overdue);

/* 逾期信息；返回 true 表示已逾期，days/hours 可为 NULL */
bool borrow_overdue_info(book item, size_t *days, size_t *hours);

/* 该学生当前已借图书快照，调用方需 delete_vector() */
vector borrow_student_books(student user);

/* 该学生已借数量（以借阅记录为准） */
size_t borrow_student_book_count(student user);

#endif // _service_borrow_h
