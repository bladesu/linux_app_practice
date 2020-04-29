/*
    Write a program to verify that duplicated file descriptors share a file offset value and open file status flags.
*/
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif
#include "lap_base.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[], char *env[])
{
    int process_status = EXIT_SUCCESS;
    int fd, fd1, fd2;
    /* 
        Here a char array is defined, a sequence of following 
        characters was copied from string literal to stack memory.
        Which is needed for mkstemp() rather than pointer to string literal,
        ßbecause it will update the char array.
    */
    char template[] = "/tmp/tempXXXXXX";
    fd = mkstemp(template);
    if (fd == -1)
    {
        printf("Fail to open temporary file.\n");
        goto fail;
    }

    fd1 = dup(fd);
    if (fd1 == -1)
    {
        printf("Fail to dup(%d)\n", fd);
        goto fail;
    }

    fd2 = dup(fd);
    if (fd2 == -1)
    {
        printf("Fail to dup(%d)\n", fd);
        goto fail;
    }

    // move offset for fd1;
    off_t offset1 = 1000;
    if (lseek(fd1, offset1, SEEK_SET) == -1)
    {
        printf("Fail to lseek for fd:%d", fd1);
        goto fail;
    }

    // get current offset in fd2;
    off_t offset2 = 0;
    if ((offset2 = lseek(fd2, 0, SEEK_CUR)) == -1)
    {
        printf("Fail to lseek for fd:%d", fd2);
        goto fail;
    }

    if (offset1 == offset2)
        printf("The offset in fd:%d and in fd:%d are the same.", fd1, fd2);
    else
        printf("The offset in fd:%d and in fd:%d are different.", fd1, fd2);

    process_status = EXIT_SUCCESS;
    goto final;

fail:
    process_status = EXIT_FAILURE;

final:
    if (fd)
        close(fd);
    if (fd1)
        close(fd1);
    if (fd2)
        close(fd2);
    if (template != NULL)
        unlink(template);
    exit(process_status);
}