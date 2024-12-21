#include <stdio.h>
#include "ui/func.h"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

void clear_screen() {
    printf("\033[H\033[J");
}

// Unix/Linux/macOS 使用 termios 禁用回显和标准输入缓冲
char getch(){
    char ch;
#ifdef _WIN32
    ch = _getch();
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return ch;
}