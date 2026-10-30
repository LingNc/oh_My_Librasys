#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "Tools/Vector.h"
#include "Tools/String.h"
#include "models/Student.h"
#include "models/Book.h"
#include "models/Manager.h"
#include "DataBase/DataBase.h"
#include "ui/components/list.h"
#include "ui/components/func.h"
#include "service/format.h"

extern dataBase studentDb, bookDb, managerDb;
extern database_index btos;

void display_wchar(int highlight,const wchar_t **lines,int n_lines){
    for (int i = 0; i < n_lines; ++i) {
        if (i == highlight) {
            printf("> %ls\n", lines[i]);
        } else {
            printf("  %ls\n", lines[i]);
        }
    }
}

void display_one_line(bool isHighlight, string type, void* data) {
    if (isHighlight) {
        printf("> ");
    } else {
        printf("  ");
    }

    char row[512];
    if (strcmp(type->c_str(type), "Student") == 0) {
        format_student_row((student)data, row, sizeof(row));
        printf("%s\n", row);
    } else if (strcmp(type->c_str(type), "Book") == 0) {
        format_book_row((book)data, row, sizeof(row));
        printf("%s\n", row);
    }
    else if(strcmp(type->c_str(type),"Manager")==0){
        format_manager_row((manager)data, row, sizeof(row));
        printf("%s\n", row);
    } else if (strcmp(type->c_str(type), "string") == 0) {
        string temp = (string)data;
        printf("%-50s\n", temp->c_str(temp));
    } else {
        printf("未知类型\n");
    }
}

void display(int highlight, vector lines) {
    for (size_t i = 0; i < lines->size(lines); ++i) {
        void *data = lines->at(lines, i);
        if (data) {
            display_one_line((size_t)highlight == i, lines->_typename, data);
        }
    }
}