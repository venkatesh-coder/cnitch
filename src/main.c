#define _GNU_SOURCE

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "recdir.h"
#include "todo.h"
#include "util.h"


int main1(void)
{
    char *file_path = "Makefile";
    FNode *fnode = HashTable_create_node(file_path);
    assert(fnode != NULL);
    HashTable_add_file_path(fnode);
    char *fp1 = "src/list.h";
    FNode *fnode1 = HashTable_create_node(fp1);
    HashTable_add_file_path(fnode1);
    FNode *f = HashTable_find_file(file_path);
    if (f != NULL)
        printf("found: %s\n", f->file_path);
    else
        printf("not found: %s\n", file_path);
    HashTable_print();
    HashTable_unload();
    return 0;
}

int main(void)
{
    char *path = strdup(".");
    RECDIR *recdirp = recdir_open(path);
    Com_entry_list *com_ent_list = NULL;

    struct dirent *ent = recdir_read(recdirp);
    while (ent)
    {
        RECDIR_Frame *top = recdir_top(recdirp);
        char *dir_path = top->path;
        uint32_t base_len = strlen(dir_path);
        char file_path[base_len + MAX_FILENAME_LEN];
        strcpy(file_path, dir_path);
        char *file_path_p = join_path(file_path, ent->d_name); 

        printf("file: %s\n", file_path_p);
        Com_mode_find_and_add_todos(&com_ent_list, file_path_p);
        ent = recdir_read(recdirp);
    }
    recdir_close(recdirp);
    Com_mode_list_print_entries(com_ent_list);
    Com_mode_list_free_entries(com_ent_list);

    free(recdirp);
    return 0;
}

/* int main(void) */
/* { */
/*     return 0; */
/* } */
