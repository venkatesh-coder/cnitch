#ifndef TODO_H
#define TODO_H

#include <stdbool.h>
#include <stdint.h>

#define ASCII_ZERO_OFFSET 48
#define ASCII_UPPER_OFFSET 65
#define ASCII_LOWER_OFFSET 97
#define H_DIGIT_OFFSET 10
#define H_UPPER_OFFSET 26
#define ALNUM_OFFSET 62
#define HASH_TABLE_SIZE (ALNUM_OFFSET + 1)

#define DEFAULT_FILE_SIZE 1024 * 8 * 10

// vim/emacs compilation mode data.
typedef struct {
    uint32_t line_no;
    uint32_t col;
} Com_mode_no_file_path;


typedef struct {
    uint32_t line_no;
    uint32_t col;
    uint8_t priority;
    char *file_path;
} Com_mode_entry;


typedef struct LCom_entry_list{
    struct LCom_entry_list *next;
    Com_mode_entry *ent;
} Com_entry_list;


typedef struct LFNode {
    struct LFNode *next;
    char *file_path;
} FNode;

Com_mode_entry Com_mode_create_ent(const char *, uint32_t, uint32_t, uint8_t);
void Com_mode_list_add_ent(Com_entry_list **, Com_entry_list *);
void Com_mode_print_entries(Com_mode_entry *);
void Com_mode_find_and_add_todos(Com_entry_list **, char *);

void Com_mode_list_print_entries(Com_entry_list *com_list);
void Com_mode_list_free_entries(Com_entry_list *com_list);

FNode * HashTable_create_node(const char *);
FNode * HashTable_add_file_path(FNode *);
uint32_t HashTable_hash_func(const char *file_path);
void HashTable_print();
FNode * HashTable_find_file(char *file_path);
void HashTable_unload();

void read_to_existing_buffer(int, char *, uint64_t);
char * read_to_new_buffer(int, uint64_t);
Com_entry_list * add_todos(Com_entry_list **, const char *, uint64_t);
bool is_update_comment(const char *str);
uint32_t is_comment(const char *str);

void add_file_path_to_appended_entries(Com_entry_list *, char *);

#endif
