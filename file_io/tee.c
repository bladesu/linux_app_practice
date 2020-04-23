#include "lap_base.h"
#include <fcntl.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif
const static int NEW_FMOD =
    S_IRUSR | S_IWUSR |
    S_IRGRP | S_IWGRP |
    S_IROTH | S_IWOTH; /* rw-rw-rw- */

int main(int argc, char *argv[], char *env[])
{
    int process_status = EXIT_SUCCESS;
    char *file_name;
    int append_flag;
    while (optind < argc)
    {
        int c;
        if ((c = getopt(argc, argv, "a")) != -1)
        {
            if (c == 'a') // this line is not necessary.
            {
                append_flag = 1;
            }
        }
        else
        {
            file_name = argv[optind];
        }

        optind++;
    }

    int fd_log;
    if (append_flag)
    {
        fd_log = open(file_name, O_CREAT | O_WRONLY | O_APPEND, NEW_FMOD);
    }
    else
    {
        fd_log = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, NEW_FMOD);
    }

    if (fd_log == -1)
    {
        printf("Fail to open file:%s\n", file_name);
        goto error_stage;
    }

    char buff[BUF_SIZE];
    int num_read;

    if ((num_read = read(STDIN_FILENO, buff, BUF_SIZE)) != -1)
    {
        printf("%.*s", BUF_SIZE, buff);
        if (write(fd_log, buff, num_read) != num_read)
        {
            printf("log writing wrong...\n");
            goto error_stage;
        }
    }

    goto final_stage;
error_stage:
    process_status = EXIT_FAILURE;
final_stage:
    if (fd_log != -1)
        close(fd_log);
    exit(process_status);
}