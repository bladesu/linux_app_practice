/*
    Implement dup() and dup2() using fcntl() and, where necessary, close(). 
    (You may ignore the fact that dup2() and fcntl() return different errno values
     for some error cases.) For dup2(), remember to handle the special case 
     where oldfd equals newfd. In this case, you should check whether oldfd is valid,
      which can be done by, for example, checking if fcntl(oldfd, F_GETFL) succeeds. 
      If oldfd is not valid, then the function should return –1 with errno set to EBADF.

*/
#include "lap_base.h"
#include <fcntl.h>
#include <sys/stat.h>

void check(int fd)
{
    if (fd == -1)
        errno = EBADF;
}

void check_old(int fd)
{
    if (fcntl(fd, F_GETFL))
        errno = EBADF;
}

int dup(int old)
{
    check(old);
    int fd = fcntl(old, F_DUPFD);
    check(fd);
    return fd;
}

int dup2(int old, int new)
{
    check(old);
    int fd = fcntl(old, F_DUPFD, old);
    check(fd);
    return fd;
}

int main(int argc, char *argv[], char *end[])
{
    char *filename = "./temp_file";

    int fd1 = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd1 == -1)
        err_exit("Fail to open %s\n", filename);

    int fd2 = dup(fd1);
    if (fd2 == -1)
        err_exit("Fail to duplicate file descriptor %d\n", fd1);

    int fd3 = dup2(fd2, fd2 + 1);
    if (fd3 == -1)
        err_exit("Fail to duplicate file descriptor %d\n", fd2);

    write(fd1, "abc\n", 4);
    write(fd2, "def\n", 4);
    write(fd3, "ghi\n", 4);

    close(fd3);
    close(fd2);
    close(fd1);

    exit(EXIT_SUCCESS);
}