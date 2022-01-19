#ifndef LIST_H
#define LIST_H

#include <stdint.h>

#include "todo.h"

typedef struct N_Node { // this name ofuscation is for cscope
    Com_mode_entry com_entry;
    struct N_Node *next;
} Node;

Node * create_com_mode_list(Com_mode_entry com_entry);
void add_file(Node *list, Com_mode_entry com_entry);
void print_list(Node *list);
void unload_list(Node *list);

#endif
