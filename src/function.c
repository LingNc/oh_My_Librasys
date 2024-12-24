#include<stdio.h>
#include<stdlib.h>
#include "models/Book.h"
#include "models/uiBook.h"
#include "models/Student.h"
#include "models/uiStudent.h"
#include "models/Manager.h"
#include "DataBase/DataBase.h"
#include "ui/components/func.h"
#include "Tools/Hash.h"

#define TEMP_NUMS 1

extern database_index btos;
extern dataBase managerDb,passwordDb;

void clock_times(const char *msg,size_t nums){
    clear_screen();
    printf("%s%zu\n",msg,nums);
}
// 从文件中批量加载书籍
bool load_books_from_file(const char *filePath, dataBase bookDb) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("load book 无法打开文件");
        perror(filePath);
        return 0;
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
        load_book(newBook, id, ISBN, name, author, publisher, time, 0);
        bookDb->add(bookDb, newBook);
        count++;
        clock_times("加载书籍数量:",count);
        if(count%TEMP_NUMS==0){
            bookDb->save(bookDb);
        }
    }

    fclose(file);
    bookDb->save(bookDb);
    return true;
}
// 从文件中批量加载学生信息
bool load_students_from_file(const char *filePath, dataBase studentDb) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        perror("load student 无法打开文件");
        perror(filePath);
        return 0;
    }

    int count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        size_t studentID;
        char name[50], class[50], department[50], borrowedDate[20], returnDate[20];
        int borrowedCount;

        sscanf(line, "%zu,%49[^,],%49[^,],%49[^,],%d,%19[^,],%19[^,]",
               &studentID, name, class, department, &borrowedCount, borrowedDate, returnDate);

        student newStudent = new_student();
        load_student(newStudent, 0, studentID, name, class, department, borrowedCount, borrowedDate, returnDate);
        studentDb->add(studentDb, newStudent);

        count++;
        clock_times("加载学生数量:",count);
        if(count%TEMP_NUMS==0){
            studentDb->save(studentDb);
        }
    }

    fclose(file);
    studentDb->save(studentDb);
    return true;
}
// 保存一个人的借阅记录
void save_borrow_records(dataBase borrowDb, size_t student_id, vector records) {
    string ser_records=new_string();
    size_t allSize=0;
    const char *data=(const char *)records->data(records);
    allSize+=*(size_t *)data;
    allSize+=sizeof(size_t);
    ser_records->append_n(ser_records,data,allSize);
    borrowDb->change(borrowDb,student_id,ser_records);

    // 更新书籍ID到学生ID的索引
    for (size_t i = 0; i < records->size(records); ++i) {
        string record = (string)records->at(records, i);
        size_t book_id;
        memcpy(&book_id, record->c_str(record), sizeof(size_t));
        add_index(btos,book_id,student_id);
        save_index(btos);
    }
}
// 从数据库中加载一个人借阅记录
vector load_borrow_records(dataBase borrowDb, size_t student_id) {
    string ser_records = borrowDb->find_key(borrowDb, student_id);
    if (!ser_records) {
        ser_records=new_string();
        size_t t=0;
        ser_records->append_n(ser_records,(const char *)&t,sizeof(size_t));
        ser_records->append_n(ser_records,(const char *)&t,sizeof(size_t));
        borrowDb->add_key(borrowDb,ser_records,student_id);
        borrowDb->save(borrowDb);
    }
    vector records = vector(String);
    records->in_data(records, ser_records->c_str(ser_records));
    return records;
}

// 初始化根管理用户
void init_root(){
    // 检测root是否存在
    manager root=managerDb->find_key(managerDb,1);
    if(!root){
        printf("正在初始化管理员\n");
        printf("默认管理员id为 1\n");
        root=new_manager();
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char date[20];
        strftime(date, sizeof(date), "%Y-%m-%d", t);
        load_manager(root, 1, "root", date, "system");
        managerDb->add(managerDb, root);
        managerDb->save(managerDb);

        // 初始化密码为"admin"
        printf("初始化密码为 admin\n");
        printf("请尽快修改密码\n");
        string default_password=new_string();
        default_password->assign_cstr(default_password, "admin");
        string hashed_password = sha256(default_password);
        passwordDb->add_key(passwordDb, hashed_password, 1);
        passwordDb->save(passwordDb);

        free_manager(root);
        printf("初始化完毕\n按任意键继续...");
        getch();
    }
}