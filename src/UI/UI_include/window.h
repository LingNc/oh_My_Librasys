#ifndef WINDOWS_H
#define WINDOWS_H

#include "utils.h"

void show_confirm_window(uiBook* book);

void show_student_confirm_window(Student* student);

Inputinfo* input_info(char* a, char* b, char *c, char*d);




void create_windows(WINDOW **wins, int height, int width, int starty, int startx);
void destroy_windows(WINDOW **wins);

void show_message_box(const char *message);


#endif