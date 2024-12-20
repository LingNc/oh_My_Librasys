#include<stdio.h>
#include<stdlib.h>
#include "Book.h"
#include "uiBook.h"
#include "Student.h"
#include "uiStudent.h"
#include "DataBase/DataBase.h"

void load_books_from_file(const char *filePath, dataBase bookDb) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("load uibook: 无法打开文件");
        return;
    }

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
        bookDb->add(bookDb, newBook);

        count++;
        if (count % 5000 == 0) {
            bookDb->save(bookDb);
        }
    }

    fclose(file);
    bookDb->save(bookDb);
}

void load_students_from_file(const char *filePath, dataBase studentDb) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("load uistudent: 无法打开文件");
        return;
    }

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
        studentDb->add(studentDb, newStudent);

        count++;
        if (count % 5000 == 0) {
            studentDb->save(studentDb);
        }
    }

    fclose(file);
    studentDb->save(studentDb);
}
void save_borrow_records(dataBase borrowDb, size_t student_id, vector records) {
    string ser_records=new_string();
    ser_records->append_char(ser_records,records->data(records));
    borrowDb->add_auto(borrowDb,ser_records);
    borrowDb->save(borrowDb);
}


vector load_borrow_records(dataBase borrowDb,size_t student_id){
    string ser_records = borrowDb->find_key(borrowDb, student_id);
    vector records=vector(String);
    records->in_data(records,ser_records->c_str(ser_records));
    return records;
}

