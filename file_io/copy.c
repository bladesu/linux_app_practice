/*
    A simple tool to copy file leverage system call: open,read,write,close
*/
#include "copy.h"

PROCESS_FLAG process_copy(FILE_STR *fs_arr)
{
    char buf[BUF_SIZE];
    ssize_t num_read = 0;
    int fd_from = fs_arr->fd;
    int fd_to = (fs_arr + 1)->fd;
    while ((num_read = read(fd_from, buf, BUF_SIZE)) > 0)
    {
        if (write(fd_to, buf, num_read) != num_read)
        {
            printf("Fail to write properly to file:%s\n", (fs_arr + 1)->name);
            return FAIL;
        }
    }
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
    process_flag = process_flag && process_copy(ft_arr);
    process_flag = process_flag && iterate_func(ft_arr, fd_arr_len, close_file);
    printf("Processe status:%s\n", PROCESS_FLAG_NAME[process_flag]);
    exit(EXIT_SUCCESS);
}
