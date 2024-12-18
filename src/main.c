#include "menu.h"
#include "UI/screen.h"
#include "UI/utils.h"
#include "uiBook.h"
#include "Book.h"
#include "function.h"

extern Screen (*screen_functions[])();

int main()
{
    Stack *stack = (Stack *)calloc(1, sizeof(Stack));
    stack->top = -1;
    Screen cur_screen = MAIN;
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
