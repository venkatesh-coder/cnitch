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

    Com_entry_list *appended_entries_start = add_update_cmnts(com_ent_list, newbuf, file_size);
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
Com_entry_list * add_update_cmnts(Com_entry_list **com_ent_list,
        const char *file_buf,
        uint64_t buf_size)
{
    Com_entry_list *ent_list_item = NULL;
    Com_entry_list *ent_list_first_item = NULL;
    uint32_t line_no = 1;

    for (uint64_t end_of_len = 0, line_len = 0, buf_cur_pos = 0;
            (buf_cur_pos < buf_size && ((line_len = buf_getline_len(file_buf, buf_cur_pos)) > 0));
            buf_cur_pos = end_of_len, line_no++)
    {
        end_of_len = line_len + buf_cur_pos;
        if (line_len < 7)
            continue;

        for (uint32_t i = buf_cur_pos, punct_chrs_cnt = 0;
                i < end_of_len;
                i++)
        {
            if (isspace(file_buf[i]) || isalnum(file_buf[i]))
            {
                punct_chrs_cnt++;
                continue;
            }

            uint32_t is_cmntlen_or_none = is_comment(file_buf + i); 
            if (is_cmntlen_or_none)
            {
                uint32_t update_cmnt_len = 0; 
                const char *cmp_str = file_buf + i + is_cmntlen_or_none;
                uint32_t cmp_str_len = line_len - is_cmntlen_or_none - punct_chrs_cnt;
                if ((update_cmnt_len = is_update_comment(cmp_str, cmp_str_len)))
                {
                    const char *updt_cmnt_stmnt = cmp_str + update_cmnt_len;
                    // TODO: xyz -> updt_cmnt_stmnt_len starting from  T to z\n
                    uint32_t updt_cmnt_stmnt_len = cmp_str_len - update_cmnt_len;

                    ent_list_item = malloc(sizeof(Com_entry_list));
                    assert(ent_list_item != NULL);
                    if (ent_list_first_item == NULL)
                        ent_list_first_item = ent_list_item;

                    ent_list_item->next = NULL;
                    Com_mode_entry *ent = malloc(sizeof(Com_mode_entry));
                    assert(ent != NULL);

                    ent_list_item->ent = ent;
                    ent->line_no = line_no;
                    ent->file_path = NULL;
                    ent->priority = find_priority(updt_cmnt_stmnt, updt_cmnt_stmnt_len);;
                    Com_mode_list_add_ent(com_ent_list, ent_list_item);
                }
            }
            else
            {
                punct_chrs_cnt++;
            }
        }
    }
    return ent_list_first_item;
}


uint32_t is_update_comment(const char *str, uint64_t len)
{
    uint64_t idx = 0;
    uint32_t sp_cnt = 0;
    // FIXME: remove sp_cnt somehow
    for (idx = 0; idx < len && isspace(str[idx]); idx++)
        sp_cnt++;
    sp_cnt--;

    str = str + idx;
    for (uint32_t i = 0; i < UPDATE_CMNTS_CNT; i++)
    {
        uint32_t cmp_len = strlen(UpdateCmnts[i]);
        if (strncmp(str, UpdateCmnts[i], cmp_len) == 0)
            return cmp_len + sp_cnt;
    }
    return 0;
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


uint32_t find_priority(const char *updt_cmnt_stmnt, uint32_t updt_cmnt_stmnt_len)
{
    uint32_t priority = 0;
    for (uint32_t j = 0; j < updt_cmnt_stmnt_len; j++)
    {
        if (isalpha(updt_cmnt_stmnt[j]))
            priority++;
        else
            break;
    }
    return priority;
}
