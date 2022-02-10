#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define PATH_SEP "/"
#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)

// this allocate memory on heap, change it to stack now.
char * join_path(char [], char []);
uint64_t get_file_size(int32_t);
char * get_file_ext(char *);
uint32_t buf_getline_len(const char *, uint64_t);
void print_nline(const char *, uint32_t);
uint32_t strline_len(const char *, uint64_t);
char * strip(char *str);

#endif
