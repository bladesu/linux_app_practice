/*
  Implementation of program insert character or read character file by moving offset.
  It could create file hole on UNIX native file system.
*/
#include "lap_base.h"
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[], char *env[])
{
  if (argc < 3 || strcmp(argv[1], "--help") == 0)
  {
    usageErr("%s -f file_name {-r bytes |-w string|-o offset} -x (show in hexadecimal format)...\n", argv[0]);
  }
  int e_status;
  size_t r_st, o_st, w_st;
  char *w_char;
  char *file_name;
  unsigned char *buff = NULL;
  int show_binary = 0;

  int c;
  while ((c = getopt(argc, argv, ":f:r:w:o:x")) != -1)
    switch (c)
    {
    case 'f':
      file_name = (char *)malloc(strlen(optarg));
      strcpy(file_name, optarg);
      break;
    case 'r':
      r_st = (size_t)atoi(optarg);
      break;
    case 'w':
      w_st = strlen(optarg);
      w_char = (char *)malloc(w_st);
      strcpy(w_char, optarg);
      break;
    case 'o':
      o_st = (size_t)atoi(optarg);
      break;
    case 'x':
      show_binary = 1;
      break;
    case '?':
      if (isprint(optopt))
      {
        fprintf(stdout, "Unknown option `-%c'.\n", optopt);
      }
      else
      {
        fprintf(stdout,
                "Unknown option character `\\x%x'.\n",
                optopt);
      }
      exit(EXIT_FAILURE);
    }

  // open file descriptor with read and write authority.
  int fd = open(file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

  // process flow
  if (fd == -1)
    errExit("Fail to open file:%s", file_name);

  // step 1: change file offset;
  if (o_st)
  {
    if (lseek(fd, o_st, SEEK_SET) == -1)
    {
      printf("Fail to lseek.\n");
      goto err_end;
    }
    printf("Success to lseek, move forward %zu bytes.\n", o_st);
  }

  // step 2: write or read
  if (w_st)
  {
    int written = write(fd, w_char, w_st);
    if (written == -1)
    {
      printf("Fail to write file.\n");
      goto err_end;
    }
    else
    {
      printf("wrote %zu byte\n", w_st);
      if (show_binary)
      {
        printf("content in binary:\n");
        char *temp = w_char;
        for (int i = 0; i < w_st; i++)
        {
          printf("%02x ", *(temp++));
        }
        printf("\n");
      }
      else
      {
        printf("content = string:\"%s\"\n", w_char);
      }

    }
  }
  else if (r_st)
  {
    buff = malloc(r_st);
    if (buff == NULL)
    {
      printf("Fail to read because of failure to malloc");
      goto err_end;
    }

    int n_read = read(fd, buff, r_st);
    printf("read %d bytes\n", n_read);
    for (int i = 0; i < n_read; i++)
    {
      if (show_binary)
        printf("%0 2x ", buff[i]);
      else
        printf("%c", isprint(buff[i]) ? buff[i] : '?');
    }
    printf("\n");
  }

  e_status = EXIT_SUCCESS;
  goto end;

err_end:
  e_status = EXIT_FAILURE;
end:
  if (buff != NULL)
    free(buff);
  if (fd != -1)
    close(fd);
  exit(e_status);
}