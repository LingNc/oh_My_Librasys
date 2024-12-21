#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tools/Trie.h"

trie create_node() {
    trie node = (trie)malloc(sizeof(Trie));
    node->is_end_of_word = 0;
    for (int i = 0; i < 128; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void insert(trie root, const char *key) {
    trie node = root;
    for (int i = 0; key[i] != '\0'; i++) {
        int index = key[i];
        if (!node->children[index]) {
            node->children[index] = create_node();
        }
        node = node->children[index];
    }
    node->is_end_of_word = 1;
}

void build_trie(trie root, const char **commands, int num_commands) {
    for (int i = 0; i < num_commands; i++) {
        insert(root, commands[i]);
    }
}

int search(trie root, const char *key) {
    trie node = root;
    for (int i = 0; key[i] != '\0'; i++) {
        int index = key[i];
        if (!node->children[index]) {
            return 0;
        }
        node = node->children[index];
    }
    return node != NULL && node->is_end_of_word;
}

int count_children(trie node, char *last_char) {
    int count = 0;
    for (int i = 0; i < 128; i++) {
        if (node->children[i]) {
            count++;
            *last_char = i;
        }
    }
    return count;
}