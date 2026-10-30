#ifndef _service_auth_h
#define _service_auth_h

#include <stddef.h>

#include "models/Manager.h"
#include "models/Student.h"

typedef enum {
    AUTH_OK = 0,
    AUTH_USER_NOT_FOUND,
    AUTH_BAD_PASSWORD,
    AUTH_PASSWORD_NOT_SET
} auth_status;

typedef struct {
    auth_status status;
    manager user;
} manager_auth_result;

manager_auth_result auth_login_manager(size_t id, const char *password);
student auth_find_student(size_t student_id);

#endif // _service_auth_h