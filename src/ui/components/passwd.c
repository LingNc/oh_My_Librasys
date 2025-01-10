#include <stdio.h>
#include "Tools/String.h"
#include "ui/components/passwd.h"
#include "ui/components/func.h"

string get_password(const char msg[]){
    int i=0;
    char ch;
    string password=new_string();

    printf("%s",msg);
    while(1){
        // 禁用回显读取字符
        ch=getch();

        if(ch=='\n'){ // 回车键结束输入
            break;
        }
        else if(ch==8||ch==127){ // 退格键
            if(i>0){
                i--;
                password->erase_char(password,i);
                printf("\b \b"); // 删除屏幕上的字符
            }
        }
        else{
            password->append_char(password,ch);
            i++;
            printf("*"); // 显示星号
        }
    }

    printf("\n");
    return password;
}
