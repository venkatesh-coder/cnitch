#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "list.h"
#include "todo.h"
#include "util.h"


#define UPDATE_CMNT_LEN 7
static char UpdateCmnts[][UPDATE_CMNT_LEN] = {"TODO", "FIXME"};
#define UPDATE_CMNTS_CNT (sizeof(UpdateCmnts) / UPDATE_CMNT_LEN)

#define CMNT_LEN 4
static char CmntsFmt[][CMNT_LEN] = {"//", "/*", "#", "--"};
#define CMNTS_FMT_CNT (sizeof(CmntsFmt) / CMNT_LEN)

Com_mode_entry Com_mode_create_ent(
        const char *file_path,
        uint32_t line_no,
        uint8_t priority)
{
    (void) file_path;
    Com_mode_entry ent;

    ent.line_no = line_no;
    ent.priority = priority;

    return ent;
}


void Com_mode_list_add_ent(Com_entry_list **com_list,
        Com_entry_list *ent_list_item)
{
    assert(com_list != NULL);
    if (*com_list == NULL)
    {
        *com_list = ent_list_item;
        return;
    }

    Com_entry_list *list = *com_list;
    while (list->next != NULL)
    {
        list = list->next;
    }
    list->next = ent_list_item;
}


void Com_mode_find_and_add_todos(
        Com_entry_list **com_ent_list,
        char *file_path)
{
    int fd = open(file_path, O_RDONLY);
    if (fd < 0)
        err(1, "Unable to open file: %s", file_path);

    struct stat statbuf;
    int64_t ret = fstat(fd, &statbuf);
    if (ret < 0)
        err(1, "Unable to file stats of %s", file_path);

    uint64_t file_size = statbuf.st_size;
    char buf[DEFAULT_FILE_SIZE];
    char *newbuf = buf;
    if (file_size < DEFAULT_FILE_SIZE)
        read_to_existing_buffer(fd, newbuf, file_size);
    else
        newbuf = read_to_new_buffer(fd, file_size);
    close(fd);

    Com_entry_list *appended_entries_start = add_todos(com_ent_list, newbuf, file_size);
    if (file_size > DEFAULT_FILE_SIZE)
        free(newbuf);
    add_file_path_to_appended_entries(appended_entries_start, file_path);
}


void Com_mode_list_print_entries(Com_entry_list *com_list)
{
    printf("list\n");
    while (com_list != NULL)
    {
        Com_mode_entry *ent = com_list->ent;
        printf("%s:%u: \n", ent->file_path, ent->line_no);
        com_list = com_list->next;
    }
}



void read_to_existing_buffer(int fd, char *buf, uint64_t file_size)
{
    int64_t ret = read(fd, buf, file_size);
    if (ret == -1)
        err(1, "Error reading file in %s()", __func__);
}


char * read_to_new_buffer(int fd, uint64_t file_size)
{
    char *buf = malloc(file_size + 1);
    read_to_existing_buffer(fd, buf, file_size);
    return buf;
}


// FIXME: this reads entire file as buffer
Com_entry_list * add_todos(Com_entry_list **com_ent_list,
        const char *file_buf,
        uint64_t buf_size)
{
    Com_entry_list *ent_list_item = NULL;
    Com_entry_list *ent_list_first_item = NULL;
    uint32_t line_no = 1;
    uint32_t col = 1;
    for (uint64_t i = 0; i < buf_size; i++)
    {
        if (file_buf[i] == '\n')
        {
            line_no++;
            col = 1;
            continue;
        }
        if (file_buf[i] == ' ' || file_buf[i] == '\t')
            continue;
        if (buf_size - i <= 3)
            break;

        uint32_t is_cmnt_or_len = is_comment(file_buf + i); 
        if (is_cmnt_or_len)
        {
            for (i = i + is_cmnt_or_len;
                    (buf_size - 1 && (file_buf[i] == ' ' || file_buf[i] == '\t'));
                    i++)
                ;
            if (buf_size - i - is_cmnt_or_len <= 6)
                break;
            if (is_update_comment(file_buf + i))
            {
                ent_list_item = malloc(sizeof(Com_entry_list));
                if (ent_list_first_item == NULL)
                    ent_list_first_item = ent_list_item;

                ent_list_item->next = NULL;
                assert(ent_list_item != NULL);
                Com_mode_entry *ent = malloc(sizeof(Com_mode_entry));
                assert(ent != NULL);

                ent_list_item->ent = ent;
                ent->line_no = line_no;
                ent->file_path = NULL;
                ent->col = col;
                ent->priority = 0;
                Com_mode_list_add_ent(com_ent_list, ent_list_item);
            }
        }
    }

    return ent_list_first_item;
}


bool is_update_comment(const char *str)
{
    for (uint32_t i = 0; i < UPDATE_CMNTS_CNT; i++)
        if (strncasecmp(str, UpdateCmnts[i], strlen(UpdateCmnts[i])) == 0)
            return true;
    return false;
}


uint32_t is_comment(const char *str)
{
    for (uint32_t i = 0; i < CMNTS_FMT_CNT; i++)
    {
        if (strncmp(CmntsFmt[i], str, strlen(CmntsFmt[i])) == 0)
            return strlen(CmntsFmt[i]);
    }
    return 0;
}


void add_file_path_to_appended_entries(Com_entry_list *ent_list, char *file_path)
{
    if (ent_list == NULL)
        return;

    FNode *hashtbl_file_nodep = HashTable_find_file(file_path);
    if (hashtbl_file_nodep == NULL)
    {
        hashtbl_file_nodep = HashTable_create_node(file_path);
        hashtbl_file_nodep = HashTable_add_new_file_path_node(hashtbl_file_nodep);
    }

    while (ent_list)
    {
        ent_list->ent->file_path = hashtbl_file_nodep->file_path;
        ent_list = ent_list->next;
    }
}


void Com_mode_list_free_entries(Com_entry_list *com_list)
{
    while (com_list != NULL)
    {
        Com_entry_list *temp = com_list;
        com_list = com_list->next;
        /* free(temp->ent->file_path); */
        free(temp->ent);
        free(temp);
    }
}
