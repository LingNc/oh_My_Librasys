#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "models/Book.h"
#include "models/uiBook.h"
#include "models/Student.h"
#include "models/uiStudent.h"
#include "models/Manager.h"
#include "DataBase/DataBase.h"
#include "ui/components/func.h"
#include "Tools/Hash.h"
#ifdef _WIN32
#include <direct.h>  // Windows 系统
#include <io.h>      // Windows 系统
#else
#include <sys/stat.h>  // POSIX 系统
#include <unistd.h>    // POSIX 系统
#endif

#define TEMP_NUMS 1

extern database_index btos,stoid;
extern dataBase managerDb,passwordDb;

void clock_times(const char *msg,size_t nums){
    cli_clear_screen();
    printf("%s%zu\n",msg,nums);
}

static bool split_import_fields(char *line, char **fields, size_t expected) {
    size_t count = 1;
    fields[0] = line;
    for (char *p = line; *p; ++p) {
        if (*p == '\r' || *p == '\n') {
            *p = '\0';
            break;
        }
        if (*p == ',') {
            *p = '\0';
            if (count == expected) return false;
            fields[count++] = p + 1;
        }
    }
    return count == expected;
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
        char *fields[5];
        if (!split_import_fields(line, fields, 5) ||
            strlen(fields[0]) >= 100 || strlen(fields[1]) >= 50 ||
            strlen(fields[2]) >= 500 || strlen(fields[3]) >= 500 ||
            strlen(fields[4]) >= 20 || !fields[0][0] || !fields[1][0] ||
            !fields[2][0] || !fields[3][0]) {
             fclose(file);
             return false;
         }

        book newBook = new_book();
        load_book(newBook, 0, fields[0], fields[1], fields[2], fields[3], fields[4], 0);
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
        char *fields[4];
        if (!split_import_fields(line, fields, 4) ||
            strlen(fields[0]) >= 32 || strlen(fields[1]) >= 50 ||
            strlen(fields[2]) >= 50 || strlen(fields[3]) >= 50 ||
            !fields[0][0] || !fields[1][0] || !fields[2][0] || !fields[3][0]) {
             fclose(file);
             return false;
         }
        char *end = NULL;
        unsigned long long parsed_id = strtoull(fields[0], &end, 10);
        if (!end || *end != '\0' || parsed_id == 0 ||
            parsed_id > (unsigned long long)(size_t)-1) {
            fclose(file);
            return false;
        }
        size_t studentID = (size_t)parsed_id;

        student newStudent = new_student();
        load_student(newStudent, 0, studentID, fields[1], fields[2], fields[3], 0, "", "");
        studentDb->add(studentDb, newStudent);

        count++;
        clock_times("加载学生数量:",count);
        if(count%TEMP_NUMS==0){
            studentDb->save(studentDb);
            // 添加id索引到学号
            add_index(stoid,studentID,newStudent->id);
            save_index(stoid);
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
        cli_getch();
    }
}



// 创建文件夹的函数
void create_folder(const char *path) {
    // 检查目录是否存在
#ifdef _WIN32
    if (_access(path, 0) == 0) {
        printf("文件夹 '%s' 已存在。\n", path);
        return;
    }
#else
    if (access(path, F_OK) == 0) {
        // printf("文件夹 '%s' 已存在。\n", path);
        return;
    }
#endif

    // 创建目录
#ifdef _WIN32
    if (_mkdir(path) == 0) {
        printf("文件夹 %s 创建成功。\n", path);
    } else {
        perror("创建 %s 文件夹失败",path);
    }
#else
    if (mkdir(path, 0755) == 0) {
        printf("文件夹 %s 创建成功。\n", path);
    } else {
        perror("创建 %s 文件夹失败");
    }
#endif
}