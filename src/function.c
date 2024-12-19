#include<stdio.h>
#include<stdlib.h>
#include "Book.h"
#include "uiBook.h"
#include "Student.h"
#include "uiStudent.h"

uibook* load_books_from_file(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("无法打开文件");
        return NULL;
    }

    uibook *books = (uibook *)malloc(1000 * sizeof(uibook)); // 假设最多有100本书
    int count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        size_t id;
        char ISBN[100], name[50], author[500], publisher[500], time[20];
        int status;

        sscanf(line, "%zu,%99[^,],%49[^,],%499[^,],%499[^,],%19[^,],%d",
               &id, ISBN, name, author, publisher, time, &status);

        book newBook = new_book();
        load_book(newBook, id, ISBN, name, author, publisher, time, status);
        books[count] = new_from_book(newBook);
        count++;
    }

    fclose(file);
    return books;
}

uistudent* load_students_from_file(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("无法打开文件");
        return NULL;
    }

    uistudent *students = (uistudent *)malloc(1000 * sizeof(uistudent)); // 假设最多有100个学生
    int count = 0;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        size_t id;
        char name[50], class[50], department[50], borrowedDate[20], returnDate[20];
        int borrowedCount;

        sscanf(line, "%zu,%49[^,],%49[^,],%49[^,],%d,%19[^,],%19[^,]",
               &id, name, class, department, &borrowedCount, borrowedDate, returnDate);

        student newStudent = new_student();
        load_student(newStudent, id, name, class, department, borrowedCount, borrowedDate, returnDate);
        students[count] = new_from_student(newStudent);
        count++;
    }

    fclose(file);
    return students;
}
