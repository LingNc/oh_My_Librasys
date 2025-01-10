#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include "ui/components/func.h"
#include <stdbool.h>
void clear_screen(){
    printf("\033[H\033[J");
}

// 启用所有鼠标事件跟踪
void enable_mouse_tracking(){
    printf("\033[?1003h\n");
    fflush(stdout);
}

// 禁用鼠标事件跟踪
void disable_mouse_tracking(){
    printf("\033[?1003l\n");
    fflush(stdout);
}

char handle_event(char ch){
    if(ch==27){
        getchar(); // 跳过 [
        switch(getchar()){
        case 'A': return 'w'; // 上箭头
        case 'B': return 's'; // 下箭头
        case 'D': return 'a'; // 左箭头
        case 'C': return 'd'; // 右箭头
        }
    }
    return ch;
}



// Unix/Linux/macOS 使用 termios 禁用回显和标准输入缓冲
char getch(){
#ifdef _WIN32
    char ch=_getch();
    if(ch==0||ch==224){
        switch(_getch()){
        case 72: return 'w'; // 上箭头
        case 80: return 's'; // 下箭头
        case 75: return 'a'; // 左箭头
        case 77: return 'd'; // 右箭头
        }
    }
    return ch;
#else
    struct termios oldt,newt;
    char ch;

    tcgetattr(STDIN_FILENO,&oldt);
    newt=oldt;
    newt.c_lflag&=~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    // enable_mouse_tracking();
    ch=getchar();
    ch=handle_event(ch);
    // disable_mouse_tracking();
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);

    return ch;
#endif
}



void backspace(char *input,int *input_index){
    if(*input_index>0){
        input[--(*input_index)]='\0';
        printf("\b \b");
    }
}

void refresh(char *input,int *input_index,const char *new_content){
    // 使用退格符删除index行
    for(int i=0; i<*input_index; i++){
        printf("\b \b");
    }
    // 打印新的内容
    printf("%s",new_content);
    // 更新缓冲区
    strcpy(input,new_content);
    *input_index=strlen(new_content);
}

bool getaline(char str[],char eof[]){
    scanf("%s",str);
    getchar();
    if(strlen(str)==1){
        int len=strlen(eof);
        for(int i=0;i<len;i++){
            if(str[0]==eof[i]){
                return false;
            }
        }
    }
    return true;
}

const char* str_time(const char* format, time_t timestamp) {
    static char buffer[100];
    struct tm *tm_info;
    time_t t = timestamp;
    tm_info = localtime(&t);
    strftime(buffer, 100, format, tm_info);
    return buffer;
}