#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

static FNode *FNodes_HashBucket[HASH_TABLE_SIZE] = {0};

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

FNode * HashTable_create_node(const char *file_path)
{
    FNode *n = malloc(sizeof(FNode));
    assert(n != NULL);
    n->file_path = strdup(file_path); 
    assert(n->file_path != NULL);
    n->next = NULL;
    return n;
}


FNode * HashTable_add_file_path(FNode *fnode)
{
    assert(fnode != NULL);
    uint32_t hash_val = HashTable_hash_func(fnode->file_path);
    printf("hash_val: %u\n", hash_val);

    FNode *bucket = FNodes_HashBucket[hash_val];
    if (bucket == NULL)
    {
        FNodes_HashBucket[hash_val] = fnode;
        return FNodes_HashBucket[hash_val];
    }

    while (bucket->next != NULL)
    {
        bucket = bucket->next;
    }
    bucket->next = fnode;
    return bucket->next;
}


uint32_t HashTable_hash_func(const char *file_path)
{
    assert(file_path != NULL);
    if (isupper(file_path[0]))
        return file_path[0] - ASCII_UPPER_OFFSET + H_DIGIT_OFFSET;
    else if (islower(file_path[0]))
        return file_path[0] - ASCII_LOWER_OFFSET + H_DIGIT_OFFSET + H_UPPER_OFFSET;
    else if (ispunct(file_path[0]))
        return ALNUM_OFFSET;
    else if (isdigit(file_path[0]))
        return file_path[0] - ASCII_ZERO_OFFSET;
    else
        assert(0 && "Unreachable");
}


void HashTable_print()
{
    for (uint32_t i = 0; i < HASH_TABLE_SIZE; i++)
    {
        FNode *bucket = FNodes_HashBucket[i];
        if (bucket == NULL)
            continue;
        while (bucket != NULL)
        {
            printf("hfilename: %s\n", bucket->file_path);
            bucket = bucket->next;
        }
    }
}


FNode * HashTable_find_file(char *file_path)
{
    uint32_t hash_val = HashTable_hash_func(file_path);
    FNode *bucket = FNodes_HashBucket[hash_val];
    if (bucket == NULL)
        return NULL;
    while (bucket != NULL)
    {
        if (strcmp(bucket->file_path, file_path) == 0)
            return bucket;
        bucket = bucket->next;
    }
    return NULL;
}


void HashTable_unload()
{
    for (uint32_t i = 0; i < HASH_TABLE_SIZE; i++)
    {
        FNode *bucket = FNodes_HashBucket[i];
        if (bucket == NULL)
            continue;
        while (bucket != NULL)
        {
            FNode *temp = bucket;
            bucket = bucket->next;
            free(temp->file_path);
            free(temp);
        }
    }
}
