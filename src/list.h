#ifndef LIST_H
#define LIST_H

#include <stdint.h>

#include "todo.h"

#define ASCII_ZERO_OFFSET 48
#define ASCII_UPPER_OFFSET 65
#define ASCII_LOWER_OFFSET 97
#define H_DIGIT_OFFSET 10
#define H_UPPER_OFFSET 26
#define ALNUM_OFFSET 62
#define HASH_TABLE_SIZE (ALNUM_OFFSET + 1)


typedef struct N_Node { // this name ofuscation is for cscope
    Com_mode_entry com_entry;
    struct N_Node *next;
} Node;


typedef struct LFNode {
    struct LFNode *next;
    char *file_path;
} FNode;


Node * create_com_mode_list(Com_mode_entry com_entry);
void add_file(Node *list, Com_mode_entry com_entry);
void print_list(Node *list);
void unload_list(Node *list);

FNode * HashTable_create_node(const char *);
FNode * HashTable_add_file_path(FNode *);
uint32_t HashTable_hash_func(const char *file_path);
void HashTable_print();
FNode * HashTable_find_file(char *file_path);
void HashTable_unload();

#endif
