#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"

// this allocate memory on heap, change it to stack now.
char * join_path(char *base, char *filename)
{
    uint32_t base_len = strlen(base);
    uint32_t filename_len = strlen(filename);
    base[base_len] = PATH_SEP[0];
    uint32_t final_len = base_len + filename_len + PATH_SEP_LEN + 1;
    uint32_t path_sep_len = PATH_SEP_LEN;
    for (uint32_t idx = base_len + path_sep_len;
            idx < final_len;
            idx++)
    {
        uint32_t new = idx - base_len - 1;
        base[idx] = filename[new];
    }
    base[final_len] = '\0';
    return base;
}


uint64_t get_file_size(int32_t filedes)
{
    struct stat statbuf;
    int ret = fstat(filedes, &statbuf);
    if (ret != 0)
        err(1, "Unable to read stats for file");
    return statbuf.st_size;
}


char * get_file_ext(char *filename)
{
    assert(filename != NULL);
    uint32_t str_len = strlen(filename);
    if (str_len <= 2)
        return NULL;

    for (int32_t i = str_len - 1; i >= 0; i--)
    {
        if (filename[i] == '.')
            return filename + i + 1;
    }
    return NULL;
}
