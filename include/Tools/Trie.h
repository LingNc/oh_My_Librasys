#ifndef _trie_h
#define _trie_h

#define MAX_COMMAND_LENGTH 100

typedef struct Trie Trie;
typedef Trie *trie;

struct Trie {
    trie children[128];
    int is_end_of_word;
};

trie create_node();
void insert(trie root, const char *key);
void build_trie(trie root, const char **commands, int num_commands);
int search(trie root,const char *key);
int count_children(trie node,char *last_char);

#endif // _trie_h