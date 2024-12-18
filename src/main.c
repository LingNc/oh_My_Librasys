#include "UI/menu.h"
#include "UI/screen.h"
#include "UI/utils.h"
#include "DataBase/DataBase.h"
#include "uiBook.h"
#include "Book.h"


extern Screen (*screen_functions[])();


int main(){
    dataBase test=database("db/test.bin",Book);
    book b=new_book();
    load_book(b,1234567,"918904271092","高数","李四","人民日报","",1);
    uibook c=new_from_book(b);
    uiBook bop[20];

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
