#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

// this allocate memory on heap, change it to stack now.
char * join_path(char base[], char filename[])
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
        uint32_t new = idx - base_len - path_sep_len;
        base[idx] = filename[new];
    }
    base[final_len] = '\0';
    return base;
}
