#ifndef TODO_H
#define TODO_H

#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_FILE_SIZE 1024 * 8 * 10

// vim/emacs compilation mode data.
typedef struct {
    uint32_t line_no;
    uint8_t priority;
    char *file_path;
} Com_mode_entry;


typedef struct LCom_entry_list{
    struct LCom_entry_list *next;
    Com_mode_entry *ent;
} Com_entry_list;


Com_mode_entry Com_mode_create_ent(const char *, uint32_t, uint8_t);
void Com_mode_list_add_ent(Com_entry_list **, Com_entry_list *);
void Com_mode_print_entries(Com_mode_entry *);
void Com_mode_find_and_add_todos(Com_entry_list **, char *);

void Com_mode_list_print_entries(Com_entry_list *);
void Com_mode_list_free_entries(Com_entry_list *);
void read_to_existing_buffer(int, char *, uint64_t);

char * read_to_new_buffer(int, uint64_t);
Com_entry_list * add_todos(Com_entry_list **, const char *, uint64_t);
uint32_t is_update_comment(const char *, uint64_t);
uint32_t is_comment(const char *);

void add_file_path_to_appended_entries(Com_entry_list *, char *);

#endif
