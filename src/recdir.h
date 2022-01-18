#ifndef RECDIR_H
#define RECDIR_H

#include <dirent.h>
#include <stdint.h>

#define MAX_FILENAME_LEN 256
#define DIRS_CAP 1024

typedef struct {
    DIR *dir;
    char *path;
} RECDIR_Frame;

typedef struct {
    RECDIR_Frame stack[DIRS_CAP];
    uint32_t stack_size;
} RECDIR;

RECDIR * recdir_open(char *);
struct dirent * recdir_read(RECDIR *);
void recdir_push(RECDIR *, char *);
void recdir_pop(RECDIR *);
RECDIR_Frame *recdir_top(RECDIR *);
void recdir_close(RECDIR *);



#endif
