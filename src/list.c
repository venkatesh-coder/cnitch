#include <assert.h>
#include <stdio.h>
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


void add_file(Node *list, Com_mode_entry com_entry)
{
    assert(list != NULL);
    Node *n = malloc(sizeof(Node));
    n->next = NULL;
    memcpy(&n->com_entry, &com_entry, sizeof(com_entry));
    while (list->next != NULL)
    {
        list = list->next;
    }
    list->next = n;
}


void print_list(Node *list)
{
    printf("List:\n");
    while (list != NULL)
    {
        printf("file_path: %s\n", list->com_entry.file_path);
        list = list->next;
    }
}


void unload_list(Node *list)
{
    while (list != NULL)
    {
        Node *temp = list;
        list = list->next;
        free(temp);
    }
}
