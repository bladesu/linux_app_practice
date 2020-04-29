/*
    A task to examine the function of "O_APPEND" tag, it will make atomic append to the end of current file end.
*/
#include "lap_base.h"
#include <fcntl.h>
#include <sys/stat.h>

const char DATA[] = {'a'};

int main(int argc, char *argv[], char *env[])
{

    char *filename;
    int to_write = 0;
    int oappend = 1;
    // loading variables
    if (argc == 2)
    {
        filename = argv[1];
    }
    else if (argc == 3)
    {
        filename = argv[1];
        to_write = atoi(argv[2]);
    }
    else if (argc == 4)
    {
        filename = argv[1];
        to_write = atoi(argv[2]);
        oappend = 0;
    }
    else
    {
        usageErr("%s filename bytes-to-write {any character to close O_APPEND}}\n", argv[0]);
    }

    printf("filename = %s\n", filename);
    printf("number to write = %d\n", to_write);
    printf("O_APPEND flag:%s\n", (oappend == 1) ? "OPEN" : "CLOSE");

    int fd;
    if (oappend)
    {
        fd = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    }
    else
    {
        fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    }

    double print_ratio = 0.05;
    double ratio;
    int i;
    for (i = 0; i < to_write; i++)
    {
        // wirte 1 byte per loop;
        write(fd, DATA, 1); 
        
        // print process progress.
        ratio = ((double)i) / to_write;
        if (ratio - print_ratio >= 0)
        {
            printf("processd %03.1f %%\n", ratio * 100);
            print_ratio = print_ratio + 0.05;
        }
    }
    printf("Finished\n");

    close(fd);

    exit(EXIT_SUCCESS);
}