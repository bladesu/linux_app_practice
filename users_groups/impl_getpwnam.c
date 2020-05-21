/*
    This is a exercise of the book "The Linux Programming Interface"
    8-2. Implement getpwnam() using setpwent(), getpwent(), and endpwent().

   http://man7.org/linux/man-pages/man3/getpwnam.3.html 
   http://man7.org/linux/man-pages/man3/endpwent.3p.html 
*/
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#ifndef NULL
#define NULL (0)
#endif

/*
 0 or ENOENT or ESRCH or EBADF or EPERM or ...
              The given name or uid was not found.

       EINTR  A signal was caught; see signal(7).

       EIO    I/O error.

       EMFILE The per-process limit on the number of open file descriptors
              has been reached.

       ENFILE The system-wide limit on the total number of open files has
              been reached.

       ENOMEM Insufficient memory to allocate passwd structure.

       ERANGE Insufficient buffer space supplied.
*/
struct passwd *getpwnam(const char *__name)
{
    struct passwd *pwd = NULL;
    struct passwd *tmp = NULL;
    setpwent();
    while ((pwd = getpwent()) != NULL)
    {
        if (strncmp(pwd->pw_name, __name, strlen(__name)) == 0)
        {
            tmp = pwd;
            break;
        }
    }
    endpwent();

    //not found
    if (tmp == NULL)
    {
        errno = 0;
    }

    return tmp;
}

static int times = 0;
void func()
{
    times++;
    if (times < 10)
        func();
    times = 0;
    return;

}
int main(void)
{
    struct passwd *pw = getpwnam("root");
    func();
    printf("name:%s, pid:%u\n", pw->pw_name, pw->pw_uid);
    
    return 0;
}