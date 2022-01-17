#ifndef UTIL_H
#define UTIL_H

#define PATH_SEP "/"
#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)

// this allocate memory on heap, change it to stack now.
char * join_path(char base[], char filename[]);

#endif

