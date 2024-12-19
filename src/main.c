#include "UI/menu.h"
#include "UI/screen.h"
#include "UI/utils.h"
#include "DataBase/DataBase.h"
#include "uiBook.h"
#include "Book.h"


extern Screen (*screen_functions[])();

// dataBase bookDb,studentDb;
// uibook *bookArray ;
// uistudent *studentArray;
int main(){
    bookDb=database("db/book.bin",Book);
    studentDb=database("db/student.bin",Student);
    //dataBase test=database("db/test.bin",Book);

    // 取前1000个图书信息
    size_t bookIndexCount = bookDb->_index->nums;

    size_t bookReadCount = bookIndexCount < 1000 ? bookIndexCount : 1000;

    if (bookReadCount > 0)
    {
        bookArray = (uibook *)malloc(bookReadCount * sizeof(uibook));
        for (size_t i = 0; i < bookReadCount; ++i)
        {
            book data = bookDb->find_key(bookDb, i);
            if (data)
            {
                bookArray[i] = new_from_book(data);
            }
        }
    }

    // 读取前1000个学生信息
    size_t studentIndexCount = studentDb->_index->nums;
    size_t studentReadCount = studentIndexCount < 1000 ? studentIndexCount : 1000;
    if (studentReadCount > 0)
    {
        studentArray = (uistudent *)malloc(studentReadCount * sizeof(uistudent));
        for (size_t i = 0; i < studentReadCount; ++i)
        {
            student data = studentDb->find_key(studentDb, i);
            if (data)
            {
                studentArray[i] = new_from_student(data);
            }
        }
    }
    Stack *stack=(Stack *)calloc(1,sizeof(Stack));
    stack->top=-1;
    Screen cur_screen=MAIN;
    init_all();
    while (1)
    {
        Screen next_screen = screen_functions[cur_screen]();
        switch (next_screen)
        {
        case BACK:
            cur_screen = pop_screen(stack);
            break;
        case EXIT:
            free(stack);
            endwin();
            return 0;
        default:
            push_screen(stack, cur_screen);
            cur_screen = next_screen;
            break;
        }
    }
}
