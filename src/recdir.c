#define _GNU_SOURCE

#include <assert.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recdir.h"
#include "util.h"

static char ignore_dir_list[][MAX_FILENAME_LEN]  = {".", "..", ".git", "build",
    ".config", "cscope"
};
#define IGNORE_DIRS_CNT (sizeof(ignore_dir_list) / MAX_FILENAME_LEN)

static char ignore_filetype_list[][MAX_FILENAME_LEN]  = {"tar", "gzip",
    "zip", "zstd", "out"};
#define IGNORE_FILES_CNT (sizeof(ignore_filetype_list) / MAX_FILENAME_LEN)


RECDIR * recdir_open(char *path)
{
    assert(path != NULL);

    RECDIR *recdirp = malloc(sizeof(RECDIR));
    assert(recdirp != NULL);
    memset(recdirp, 0, sizeof(RECDIR));

    DIR ** dirp = &recdirp->stack[0].dir;
    recdirp->stack[0].path = path;
    *dirp = opendir(path);
    if (dirp == NULL)
        err(1, "Unable to open directory %s", path);

    recdirp->stack_size++;
    return recdirp;
}


void recdir_push(RECDIR *recdirp, char *path)
{
    assert(recdirp != NULL);
    assert(path != NULL);
    DIR *dirp = opendir(path);
    if (dirp == NULL)
        err(1, "Unable to open directory: %s", path);

    assert(recdirp->stack_size < DIRS_CAP);
    recdirp->stack[recdirp->stack_size].dir = dirp;
    recdirp->stack[recdirp->stack_size].path = path;
    recdirp->stack_size++;
}


void recdir_pop(RECDIR *recdirp)
{
    assert(recdirp->stack_size > 0);
    RECDIR_Frame *top = recdir_top(recdirp);
    int ret = closedir(top->dir);
    assert(ret == 0);
    free(top->path);
    recdirp->stack_size--;
}


struct dirent * recdir_read(RECDIR *recdirp)
{
    assert(recdirp != NULL);

    errno = 0;
    while (recdirp->stack_size > 0)
    {
        RECDIR_Frame *top = recdir_top(recdirp);
        struct dirent *ent = readdir(top->dir);
        if (ent)
        {
            char *filename = ent->d_name;
            if (ent->d_type == DT_DIR)
            {
                bool is_ignore = false;
                // Ignore the some directories
                for (uint32_t i = 0; i < IGNORE_DIRS_CNT; i++)
                {
                    if (strcmp(ignore_dir_list[i], filename) == 0)
                    {
                        is_ignore = true;
                        break;
                    }
                }
                if (!is_ignore)
                {
                    uint32_t base_len = strlen(top->path);
                    char file_path[base_len + MAX_FILENAME_LEN];
                    strcpy(file_path, top->path);
                    char *filepathp = (char *) file_path;
                    filepathp = join_path(file_path, ent->d_name);
                    recdir_push(recdirp, strdup(filepathp));
                }
            }
            else if (ent->d_type == DT_REG)
            {
                char *ext = get_file_ext(filename);
                if (ext == NULL)
                    return ent;
                bool is_ignore = false;
                // Ignore the some files based on extension
                for (uint32_t i = 0; i < IGNORE_FILES_CNT; i++)
                {
                    if (strcmp(ignore_filetype_list[i], ext) == 0)
                    {
                        is_ignore = true;
                        break;
                    }
                }
                if (!is_ignore)
                    return ent;
            }
            else
            {
                // skip the other types of files
                fprintf(stderr, "Invalid file: %s\n", filename);
                continue;
            }
        }
        else
        {
            if (errno != 0)
                err(1, "Error while reading the directory: %s",
                        top->path);
            else
                recdir_pop(recdirp);
        }
    }
    return NULL;
}


RECDIR_Frame *recdir_top(RECDIR *recdirp)
{
    assert(recdirp != NULL);
    assert(recdirp->stack_size > 0);
    return &recdirp->stack[recdirp->stack_size - 1];
}


void recdir_close(RECDIR *recdirp)
{
    while (recdirp->stack_size > 0)
    {
        recdir_pop(recdirp);
    }
}
