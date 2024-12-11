#include<stdio.h>
#include"../include/String.h"
// #include"String.c"
int main(){
    String str;
    string_init(&str);
    str.write_cstr(&str,"1000");
    char s[]="120";
    str.append_cstr(&str,s);
    printf("%s",str.c_str(&str));
    return 0;
}