/*
    implementation of a simple version of cp, which copy file with optimization with existence file hole.
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "copy.h"

typedef struct _BLOCK
{
    off_t begin;
    off_t end;
    struct _BLOCK *next;
} BLOCK;

BLOCK *genBLOCK(off_t begin, off_t end)
{
    BLOCK *b = malloc(sizeof(BLOCK));
    b->begin = begin;
    b->end = end;
    return b;
}

BLOCK *parse(int fd)
{
    struct stat status;
    fstat(fd, &status);
    const off_t size = status.st_size;
    off_t begin, end;

    if ((begin = lseek(fd, 0, SEEK_DATA)) == -1)
        err_exit("Fail to lseek");
    if ((end = lseek(fd, begin, SEEK_HOLE)) == -1)
        err_exit("Fail to lseek");

    BLOCK *head = genBLOCK(begin, end);
    BLOCK *last = head;
    while (end < size)
    {
        if ((begin = lseek(fd, end, SEEK_DATA)) == -1)
            err_exit("Fail to lseek");
        if ((end = lseek(fd, begin, SEEK_HOLE)) == -1)
            err_exit("Fail to lseek");
        BLOCK *block = genBLOCK(begin, end);
        ;
        last->next = block;
        last = block;
    }
    return head;
}
void release(BLOCK *cursor)
{
    BLOCK *next;
    do
    {
        next = cursor->next;
        free(cursor);
        cursor = next;
    } while (next);
}

PROCESS_FLAG process_copy2(FILE_STR *fs_arr)
{
    char buf[BUF_SIZE];
    int fd_from = fs_arr->fd;
    int fd_to = (fs_arr + 1)->fd;

    BLOCK *blocks = parse(fd_from);
    BLOCK *cursor = blocks;

    do
    {
        if (lseek(fd_from, cursor->begin, SEEK_SET) == -1)
            err_exit("Fail to lseek to file:%s\n", fs_arr->name);
        if (lseek(fd_to, cursor->begin, SEEK_SET) == -1)
            err_exit("Fail to lseek to file:%s\n", (fs_arr + 1)->name);
        ssize_t written = 0;
        ssize_t limit = cursor->end - cursor->begin;
        ssize_t to_read = (limit < BUF_SIZE) ? limit : BUF_SIZE;
        while (1)
        {

            if (to_read != read(fd_from, buf, to_read))
            {
                printf("File reading broken...\n");
                return FAIL;
            }

            if (to_read != write(fd_to, buf, to_read))
            {
                printf("Fail to write properly to file:%s\n", (fs_arr + 1)->name);
                return FAIL;
            }
            written = written + to_read;
            ssize_t test = limit - written;
            if (test == 0)
                break;
            else if (test < to_read)
                to_read = test;
        }
        cursor = cursor->next;
    } while (cursor);

    release(blocks);
    return PASS;
}

int main(int argc, char *argv[], char *env[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s old-file new-file\n", argv[0]);

    int fd_arr_len = 2;

    // open files by name
    FILE_STR ft_arr[] = {
        {.name = argv[1], open(argv[1], O_RDONLY)},
        {.name = argv[2], open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, NEW_FMOD)}};

    unsigned int process_flag = PASS;

    // check opend properly
    process_flag = process_flag && iterate_func(ft_arr, fd_arr_len, check_opened);
    process_flag = process_flag && process_copy2(ft_arr);
    process_flag = process_flag && iterate_func(ft_arr, fd_arr_len, close_file);
    printf("Processe status:%s\n", PROCESS_FLAG_NAME[process_flag]);
    exit(EXIT_SUCCESS);
}
