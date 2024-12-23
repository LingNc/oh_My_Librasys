#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "Tools/Vector.h"
#include "models/Student.h"
#include "models/Book.h"
#include "models/Manager.h"
#include "ui/components/list.h"

void display_wchar(int highlight, const wchar_t **lines, int n_lines) {
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

    if (strcmp(type->c_str(type), "Student") == 0) {
        student temp = (student)data;
        printf("%-10zu %-20s %-12s %-12s %-10d\n", temp->id, temp->name->c_str(temp->name), temp->class->c_str(temp->class), temp->department->c_str(temp->department), temp->borrowedCount);
    } else if (strcmp(type->c_str(type), "Book") == 0) {
        book temp = (book)data;
        printf("%-20s %-30s %-20s %-20s %-10d\n", temp->ISBN->c_str(temp->ISBN), temp->name->c_str(temp->name), temp->author->c_str(temp->author), temp->publisher->c_str(temp->publisher), temp->status);
    } else if (strcmp(type->c_str(type), "Manager") == 0) {
        manager temp = (manager)data;
        printf("%-10zu %-20s %-20s\n", temp->id, temp->name->c_str(temp->name), temp->registration_date->c_str(temp->registration_date));
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