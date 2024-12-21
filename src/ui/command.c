#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui/command.h"
#include "Tools/Trie.h"
#include "ui/func.h"

#define NUM_COMMANDS 2

const char *commands[NUM_COMMANDS] = {
    "set ",
    "help"
};

void find_words(trie node, char *prefix, int length) {
    if (node->is_end_of_word) {
        printf("%s\n", prefix);
    }
    for (int i = 0; i < 128; i++) {
        if (node->children[i]) {
            prefix[length] = i;
            prefix[length + 1] = '\0';
            find_words(node->children[i], prefix, length + 1);
        }
    }
}

void autocomplete(trie root, char *prefix) {
    trie node = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        int index = (unsigned char)prefix[i];
        if (!node->children[index]) {
            return;
        }
        node = node->children[index];
    }

    char last_char;
    int children_count = count_children(node, &last_char);

    if (children_count == 1) {
        int len = strlen(prefix);
        prefix[len] = last_char;
        prefix[len + 1] = '\0';
        autocomplete(node->children[(unsigned char)last_char], prefix);
    } else {
        printf("\n");
        find_words(node, prefix, strlen(prefix));
    }
}

void execute(int *page_size) {
    trie root = create_node();
    build_trie(root, commands, NUM_COMMANDS);

    char input[MAX_COMMAND_LENGTH] = {0};
    int input_index = 0;
    char ch;

    printf("/");
    while ((ch = getch()) != '\n') {
        if (ch == '\t') {
            input[input_index] = '\0';
            autocomplete(root, input);
            clear_screen();
            printf("/%s", input);
        } else if (ch == 127) { // 处理退格键
            if (input_index > 0) {
                input[--input_index] = '\0';
                printf("\b \b");
            }
        } else {
            if (input_index < MAX_COMMAND_LENGTH - 1) {
                input[input_index++] = ch;
                putchar(ch);
            }
        }
    }
    input[input_index] = '\0';

    if (search(root, input)) {
        if (strncmp(input, "set ", 4) == 0) {
            *page_size = atoi(input + 4);
            printf("\n每页显示数量已设置为 %d\n", *page_size);
        } else if (strcmp(input, "help") == 0) {
            printf("\n/set [nums] - 调整每一页显示的数量\n");
        }
    } else {
        printf("\n无效命令\n");
    }
}