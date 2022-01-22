#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define PATH_SEP "/"
#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)

// this allocate memory on heap, change it to stack now.
char * join_path(char base[], char filename[]);
uint64_t get_file_size(int32_t filedes);
char * get_file_ext(char *filename);

#endif

