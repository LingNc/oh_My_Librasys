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
#ifdef _WIN32
    char ch = _getch();
    if (ch == 0 || ch == 224) {
        switch (_getch()) {
        case 72: return 'w'; // 上箭头
        case 80: return 's'; // 下箭头
        case 75: return 'a'; // 左箭头
        case 77: return 'd'; // 右箭头
        }
    }
    return ch;
#else
    struct termios oldt, newt;
    char ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    if (ch == 27) {
        getchar(); // 跳过 [
        switch (getchar()) {
        case 'A': ch = 'w'; break; // 上箭头
        case 'B': ch = 's'; break; // 下箭头
        case 'D': ch = 'a'; break; // 左箭头
        case 'C': ch = 'd'; break; // 右箭头
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
#endif
}