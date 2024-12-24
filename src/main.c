#include <stdio.h>
#include <locale.h>
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "ui/main_menu.h"

dataBase bookDb, studentDb, borrowDb, managerDb;
database_index btos; // 声明索引库

int main(){
    // 使用系统默认语言
    setlocale(LC_ALL,"");

    // 初始化数据库
    bookDb = database("db/book", Book);
    studentDb = database("db/student", Student);
    borrowDb = database("db/borrow_records", String);
    managerDb=database("db/manager",Manager);

    // 初始化bookid到studentid 索引库
    btos=new_index("db/btos");

    // 初始化自动补全
    init_autocomplete();

    // 初始化根管理用户
    init_root();

    // 进入主菜单
    main_menu();

    // 释放数据库
    close_database(bookDb);
    close_database(studentDb);
    close_database(borrowDb);
    close_database(managerDb);

    // 释放索引库
    free_index(btos);

    return 0;
}
