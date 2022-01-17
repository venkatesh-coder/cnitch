#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "recdir.h"
#include "util.h"

int main(void)
{
    Com_mode_entry com_entry;
    com_entry.line_no = 110;
    com_entry.col = 4;
    com_entry.priority = 1;
    com_entry.file_path = "./src/main.c";
    Node *com_list = create_com_mode_list(com_entry);
    printf("com:path: %s\n", com_list->com_entry.file_path);

    return 0;
}

int main1(void)
{
    char *path = strdup(".");
    RECDIR *recdirp = recdir_open(path);

    struct dirent *ent = recdir_read(recdirp);
    while (ent)
    {
        printf("file: %s\n", ent->d_name);
        ent = recdir_read(recdirp);
    }
    recdir_close(recdirp);

    free(recdirp);
    return 0;
}
