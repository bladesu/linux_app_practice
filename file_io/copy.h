#ifndef COPY_H
#define COPY_H

#include "lap_base.h"
#include "copy.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

const static int NEW_FMOD =
    S_IRUSR | S_IWUSR |
    S_IRGRP | S_IWGRP |
    S_IROTH | S_IWOTH; /* rw-rw-rw- */

typedef struct _FILE_STR
{
    char *name;
    int fd;
} FILE_STR;


typedef enum _PROCESS_FLAG
{
    FAIL,
    PASS
} PROCESS_FLAG;

char *PROCESS_FLAG_NAME[] = {"FAIL", "PASS"};

PROCESS_FLAG close_file(FILE_STR fs)
{
    if (fs.fd != 0)
    {
        if (close(fs.fd) == -1)
        {
            printf("Fail to close file:%s\n", fs.name);
            return FAIL;
        }
        printf("Successfully close file:%s\n", fs.name);
        return PASS;
    }
    printf("No proper file description to close file:%s\n", fs.name);
    return FAIL;
}

PROCESS_FLAG check_opened(FILE_STR fs)
{
    if (fs.fd == -1)
    {
        printf("Fail to open file:%s\n.", fs.name);
        return FAIL;
    }
    printf("File:%s is opend.\n", fs.name);
    return PASS;
}

PROCESS_FLAG iterate_func(FILE_STR *fs_arr, int len, PROCESS_FLAG (*fun_ptr)(FILE_STR))
{
    PROCESS_FLAG pflag = PASS;
    for (int i = 0; i < len; i++)
    {
        pflag = (*fun_ptr)(*(fs_arr + i)) && pflag;
    }
    return pflag;
}

#endif