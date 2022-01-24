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
        char file_path[base_len + MAX_FILENAME_LEN + 2];
        strcpy(file_path, dir_path);
        char *file_path_p = join_path(file_path, ent->d_name); 

        Com_mode_find_and_add_todos(&com_ent_list, file_path_p);
        ent = recdir_read(recdirp);
    }
    recdir_close(recdirp);
    Com_mode_list_sort_by_priority(com_ent_list);
    Com_mode_list_print_entries(com_ent_list);

    HashTable_unload();     // this uses global structure.
    Com_mode_list_free_entries(com_ent_list);

    free(recdirp);
    return 0;
}
