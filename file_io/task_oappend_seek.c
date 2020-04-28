/*
    implementation of a task to see how do lseek work for a file opened with O_APPEND flag.
*/
#include "lap_base.h"
#include <fcntl.h>
#include <sys/stat.h>

void show_content(char *filename)
{
    int fd = open(filename, O_RDONLY);
    int len = 1024;
    char buff[len];
    int num_read;
    while ((num_read = read(fd, buff, len)) > 0)
        printf("cotents    :<%.*s>\n", num_read, buff);
    close(fd);
}

int main(int argc, char *argv[], char *env[])
{
    char *temp_file = "./test_file";
    char temp_contents1[] = {'a', 'b', 'c'};
    char temp_contents2[] = {'x'};
    int fd;

    // remove existed temp_file
    unlink(temp_file);

    // 1.1 create a file with temp_contents1.
    fd = open(temp_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    write(fd, temp_contents1, sizeof(temp_contents1));
    close(fd);
    // 1.2 show current content
    show_content(temp_file);

    // 2.1 move offset to beginning, and write character.
    fd = open(temp_file, O_RDWR | O_APPEND);
    // move offset to beginning of file
    lseek(fd, 0, SEEK_SET);
    write(fd, temp_contents2, sizeof(temp_contents2));
    close(fd);

    // 2.2 see what happened.
    show_content(temp_file);

    exit(EXIT_SUCCESS);
}