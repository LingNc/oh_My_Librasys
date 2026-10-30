#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "DataBase/DataBase.h"
#include "models/Book.h"
#include "models/Student.h"
#include "models/Manager.h"
#include "ui/main_menu.h"
#include "ui/command.h"
#include "function.h"
#ifdef HAVE_TUI
#include "ui/tui/tui.h"
#endif

// 数据库
dataBase bookDb,studentDb,borrowDb,managerDb,passwordDb;
// 索引库
database_index btos,stoid;

int main(int argc, char **argv){
    // 使用系统默认语言
    setlocale(LC_ALL,"");
    // 初始化数据库文件夹
    create_folder("db");
    
    // 初始化数据库
    bookDb = database("db/book", Book);
    studentDb = database("db/student", Student);
    borrowDb = database("db/borrow_records", String);
    managerDb=database("db/manager",Manager);
    passwordDb = database("db/password", String);

    // 初始化bookid到studentid索引库
    btos=new_index("db/btos");
    // 初始化id到studentid索引库
    stoid=new_index("db/stoid");

    // 初始化自动补全
    init_autocomplete();

    // 初始化根管理用户
    init_root();

#ifdef HAVE_TUI
    // TUI 模式：失败时回退到 CLI，避免把用户困在坏终端里
    if (argc > 1 && strcmp(argv[1], "--tui") == 0) {
        if (tui_run() == 0) {
            return 0;
        }
        fprintf(stderr, "TUI 启动失败，回退到命令行界面\n");
    }
#else
    (void)argc;
    (void)argv;
#endif

    // 进入主菜单
    main_menu();

    // // 释放数据库
    // close_database(bookDb);
    // close_database(studentDb);
    // close_database(borrowDb);
    // close_database(managerDb);

    // // 释放索引库
    // close_index(btos);

    return 0;
}
