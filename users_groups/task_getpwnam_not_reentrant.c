/*
    This is a exercise of "The linux programing interface", 
    Please refer to the errata page if you are reading the book, there is something wrong about the original question
    (http://man7.org/tlpi/errata/)
    Here is the question:

8-1. When we execute the following code, which attempts to display the usernames for two different user IDs, 
    we find that it displays the same username twice. Why is this?

    printf("%s %s\n", getpwuid(uid1)->pw_name,
                      getpwuid(uid2)->pw_name);
    My commments:
    1. The order of 
*/

#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
int main(void)
{

    // hard code uid: 0 and 1
    uid_t uid1 = 0, uid2 = 1;
    printf("Original presentation, two pw_names are the same for different uid: %s %s\n", getpwuid(uid1)->pw_name,
           getpwuid(uid2)->pw_name);

    printf("My Comments:\n");
    printf("(1) getwuid is not reentrant, calling getwuid twice will get the same static allocated address.\n");
    struct passwd *passwd1 = getpwuid(uid1);
    struct passwd *passwd2 = getpwuid(uid2);
    printf("    Show address of the two called passwd object: %p %p\n", passwd1, passwd2);

    passwd1 = getpwuid(uid1);
    char *name1 = (char *)malloc(strlen(passwd1->pw_name) * sizeof(char));
    name1 = strcpy(name1, passwd1->pw_name);

    passwd2 = getpwuid(uid2);
    char *name2 = (char *)malloc(strlen(passwd2->pw_name) * sizeof(char));
    name2 = strcpy(name2, passwd2->pw_name);
    printf("(2) Although the pw_names string has different values when getwuid was called, the latter will replace the data to address by static allocation.\n");
    printf("    Show copied pw_name for uid=%u and uid=%u: %s and %s\n", uid1, uid2, name1, name2);

    printf("    Show same address of the returned struct member \"pw_name\" by getpwuid(), %p %p\n", getpwuid(uid1)->pw_name, getpwuid(uid2)->pw_name);
    free(name1);
    free(name2);
    return 0;
}
// ref:
// http://man7.org/linux/man-pages/man3/getpwnam.3.html