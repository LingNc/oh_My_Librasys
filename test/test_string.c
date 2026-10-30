
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tools/String.h"

void test_string_creation() {
    string s = new_string();
    assert(s->length(s) == 0);
    assert(s->empty(s) == 1);
    s->free(s);
}

void test_string_append() {
    string s = new_string();
    s->append_cstr(s, "Hello");
    assert(s->length(s) == 5);
    s->append_cstr(s, " World");
    assert(s->length(s) == 11);
    assert(strcmp(s->c_str(s), "Hello World") == 0);
    s->free(s);
}

void test_string_insert() {
    string s = new_string();
    s->append_cstr(s, "Hello World");
    s->insert_cstr(s, 6, "Beautiful ");
    assert(strcmp(s->c_str(s), "Hello Beautiful World") == 0);
    s->free(s);
}

void test_string_erase() {
    string s = new_string();
    s->append_cstr(s, "Hello Beautiful World");
    s->erase(s, 6, 10);
    assert(strcmp(s->c_str(s), "Hello World") == 0);
    s->free(s);
}

void test_string_find() {
    string s = new_string();
    s->append_cstr(s, "Hello World");
    assert(s->find_cstr(s, "World", 0) == 6);
    assert(s->find_cstr(s, "world", 0) == s->npos);
    s->free(s);
}

int main() {
    test_string_creation();
    test_string_append();
    test_string_insert();
    test_string_erase();
    test_string_find();
    printf("All string tests passed!\n");
    return 0;
}