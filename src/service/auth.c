#include "service/auth.h"

#include "DataBase/DataBase.h"
#include "DataBase/Index.h"
#include "Tools/Hash.h"
#include "Tools/String.h"

extern dataBase managerDb, studentDb, passwordDb;
extern database_index stoid;

manager_auth_result auth_login_manager(size_t id, const char *password) {
    manager_auth_result result = { AUTH_USER_NOT_FOUND, NULL };
    manager user = managerDb->find_key(managerDb, id);
    if (!user) return result;

    string stored_hash = passwordDb->find_key(passwordDb, id);
    if (!stored_hash) {
        result.status = AUTH_PASSWORD_NOT_SET;
        return result;
    }

    string input = new_string();
    input->assign_cstr(input, password ? password : "");
    string input_hash = sha256(input);

    result.status = input_hash->cmp(input_hash, stored_hash) == 0
        ? AUTH_OK
        : AUTH_BAD_PASSWORD;
    result.user = result.status == AUTH_OK ? user : NULL;
    input_hash->free(input_hash);
    input->free(input);
    return result;
}

student auth_find_student(size_t student_id) {
    size_t id = find_index(stoid, student_id);
    return studentDb->find_key(studentDb, id);
}