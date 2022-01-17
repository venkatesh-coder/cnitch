#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct {
    uint32_t line_no;
    uint32_t col;
    uint8_t priority;
    char *file_path;
} Com_mode_entry;

typedef struct N_Node { // this name ofuscation is for cscope
    Com_mode_entry com_entry;
    struct N_Node *next;
} Node;

Node * create_com_mode_list(Com_mode_entry com_entry);
void add_file(Node *list, Com_mode_entry com_entry);
void print_list(Node *list);
void unload_list(Node *list);

#endif
