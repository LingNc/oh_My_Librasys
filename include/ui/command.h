#ifndef _command_h
#define _command_h

#include <stdbool.h>
#include "Tools/Trie.h"

void execute(void **args);
void init_autocomplete();
void autocomplete(trie root, char *input, bool *show_matches, int *index, char *prefix);
void search_keyword(trie root, char *input, bool *found, int *index);

#endif // _command_h