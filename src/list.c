#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

Node * create_com_mode_list(Com_mode_entry com_entry)
{
    Node *list = malloc(sizeof(Node));
    assert(list != NULL);

    memcpy(&list->com_entry, &com_entry, sizeof(com_entry));
    list->next = NULL;
    return list;
}


void add_file(Node *list, Com_mode_entry com_entry);
void print_list(Node *list);
void unload_list(Node *list);
