/*
    Implement readv() and writev() using read(), write(), and suitable functions from the malloc package
*/
#include "lap_base.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <assert.h>

size_t _get_total(const struct iovec *iov, int iovcnt)
{
    size_t total = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        total += iov->iov_len;
        iov++;
    }
    return total;
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    size_t total = _get_total(iov, iovcnt);
    void *buff = malloc(total);
    ssize_t num_read = read(fd, buff, total);
    void *_cursor = buff;
    for (int i = 0; i < iovcnt; i++)
    {
        memcpy(iov->iov_base, _cursor, iov->iov_len);
        _cursor = _cursor + iov->iov_len;
        iov++;
    }
    free(buff);
    return num_read;
}
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    size_t total = _get_total(iov, iovcnt);
    void *buff = malloc(total);
    void *_cursor = buff;
    for (int i = 0; i < iovcnt; i++)
    {
        memcpy(_cursor, iov->iov_base, iov->iov_len);
        _cursor = _cursor + iov->iov_len;
        iov++;
    }
    ssize_t num_write = write(fd, buff, total);
    free(buff);
    return num_write;
}
int main(int argc, char *argv[])
{
    int fd;
    struct iovec iov1[3];
    typedef struct _test
    {
        int a;
    } test;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);
    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1)
        errExit("open");

    // first data.
    test myStruct = {.a = 1};
    // second data.
    int x = 2;
    // third data.
    char str[] = "abc";

    ssize_t totRequired = 0;
    iov1[0].iov_base = &myStruct;
    iov1[0].iov_len = sizeof(test);
    totRequired += iov1[0].iov_len;
    iov1[1].iov_base = &x;
    iov1[1].iov_len = sizeof(x);
    totRequired += iov1[1].iov_len;
    iov1[2].iov_base = str;
    iov1[2].iov_len = sizeof(str);
    totRequired += iov1[2].iov_len;

    printf("Apply writev\n");
    ssize_t numWrite = writev(fd, iov1, 3);
    if (numWrite == -1)
        errExit("writev");

    // first data.
    test myStruct2 = {.a = 0};
    // second data.
    int x2;
    // third data.
    char str2[] = "xyz";

    struct iovec iov2[3];
    iov2[0].iov_base = &myStruct2;
    iov2[1].iov_base = &x2;
    iov2[2].iov_base = str2;
    iov2[0].iov_len = sizeof(test);
    iov2[1].iov_len = sizeof(x);
    iov2[2].iov_len = sizeof(str2);

    printf("Apply readv\n");
    lseek(fd, 0, SEEK_SET);
    ssize_t numRead = readv(fd, iov2, 3);
    printf("readv finished.\n");

    // check elements is copied properly.
    assert(((test *)iov1[0].iov_base)->a == ((test *)iov2[0].iov_base)->a);
    assert(*((int *)iov1[1].iov_base) == *((int *)iov2[1].iov_base));
    assert(0 == strcmp(((char *)iov1[2].iov_base), ((char *)iov2[2].iov_base)));

    if (numRead == -1)
        errExit("readv");

    if (numRead < totRequired)
        printf("Read fewer bytes than requested\n");
    printf("total bytes requested: %ld; bytes read: %ld\n", (long)totRequired, (long)numRead);
    exit(EXIT_SUCCESS);
}