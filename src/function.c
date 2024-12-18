#include<stdio.h>
#include<stdlib.h>
#include "Book.h"
#include "uiBook.h"

uibook* load_books_from_file(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("无法打开文件");
        return NULL;
    }

    uibook *books = (uibook *)malloc(100 * sizeof(uibook)); // 假设最多有100本书
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
