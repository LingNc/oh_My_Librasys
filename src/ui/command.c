#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui/command.h"

void execute(int *page_size) {
    char input[100];
    printf("/");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // 去掉换行符

    if (strncmp(input, "set ", 4) == 0) {
        *page_size = atoi(input + 4);
        printf("\n每页显示数量已设置为 %d\n", *page_size);
    } else if (strcmp(input, "help") == 0) {
        printf("\n/set [nums] - 调整每一页显示的数量\n");
    } else {
        printf("\ncommand: 命令 %s 未找到。\n",input);
    }
}