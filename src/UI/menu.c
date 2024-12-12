#include <ncurses.h>

void display_main_menu()
{   
    mvprintw(4, 5, "                __         __  ___         __    _ __               _____              \n");
    mvprintw(5, 5, "         ____  / /_       /  |/  /_  __   / /   (_) /_  _________ _/ ___/__  _______   \n");
    mvprintw(6, 5, "        / __ \\/ __ \\     / /|_/ / / / /  / /   / / __ \\/ ___/ __ `/\\__ \\/ / / / ___/   \n");
    mvprintw(7, 5, "       / /_/ / / / /    / /  / / /_/ /  / /___/ / /_/ / /  / /_/ /___/ / /_/ (__  )    \n");
    mvprintw(8, 5, "       \\____/_/ /_/    /_/  /_/\\__, /  /_____/_/_.___/_/   \\__,_//____/\\__, /____/     \n");
    mvprintw(9, 5, "                              /____/                                  /____/           \n");
}

int main()
{
    initscr();
    
    display_main_menu();
    refresh();			/* Print it on to the real screen */
	getch();			/* Wait for user input */
	endwin();
}