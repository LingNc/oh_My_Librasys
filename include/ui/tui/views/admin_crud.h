#ifndef _tui_view_admin_crud_h
#define _tui_view_admin_crud_h

#include "models/Manager.h"

void tui_admin_books(manager current);
void tui_admin_students(manager current);
void tui_admin_managers(manager current);
void tui_admin_add_book(manager current);
void tui_admin_add_student(manager current);
void tui_admin_add_manager(manager current);
void tui_admin_change_password(manager current);
void tui_admin_import_books(manager current);
void tui_admin_import_students(manager current);

#endif // _tui_view_admin_crud_h