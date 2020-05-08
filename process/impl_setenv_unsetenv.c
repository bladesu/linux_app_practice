#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
/*
RETURN VALUE
       The setenv() function returns zero on success, or -1 on error, with
       errno set to indicate the cause of the error.
ERRORS
       ENOMEM Insufficient memory to add a new variable to the environment.
*/
int setenv1(const char *name, const char *value, int overwrite)
{
    // check should the value be overwrite.
    char *value_ori = getenv(name);
    if (value_ori != NULL && strcmp(value_ori, value) == 0)
        return 0;

    char str[strlen(name) + strlen(value) + 2];

    if (sprintf(str, "%s=%s", name, value) < 0)
        printf("Fail to create string\n");

    if (value_ori)
    {
        if (overwrite == 0 || putenv(str) != 0)
            return -1;
        return 0;
    }
    else
    {
        if (putenv(str) != 0)
            return -1;
        return 0;
    }
}

/*
RETURN VALUE
       The setenv() function returns zero on success, or -1 on error, with
       errno set to indicate the cause of the error.
ERRORS
       ENOMEM Insufficient memory to add a new variable to the environment.
*/
int setenv2(const char *name, const char *value, int overwrite)
{
    // check should the value be overwrite.
    char *value_ori = getenv(name);
    if (value_ori != NULL && strcmp(value_ori, value) == 0)
        return 0;

    char *str = malloc(strlen(name) + strlen(value) + 2);
    if (str == NULL || sprintf(str, "%s=%s", name, value) < 0)
    {
        errno = ENOMEM;
        return -1;
    }

    if (value_ori)
    {
        if (overwrite == 0 || putenv(str) != 0)
            return -1;
        return 0;
    }
    else
    {
        if (putenv(str) != 0)
            return -1;
        return 0;
    }
}

/*
RETURN VALUE
       The setenv() function returns zero on success, or -1 on error, with
       errno set to indicate the cause of the error.

       The unsetenv() function returns zero on success, or -1 on error, with
       errno set to indicate the cause of the error.
ERRORS
       EINVAL name is NULL, points to a string of length 0, or contains an
              '=' character.

       ENOMEM Insufficient memory to add a new variable to the environment.
*/
int unsetenv(const char *name)
{

    if (name[0] == '\0' || strchr(name, '=') != NULL)
    {
        errno = EINVAL;
        return -1;
    }

    typedef struct _tmp
    {
        char *str;
        struct _tmp *next;
    } env_variable;

    env_variable *head = malloc(sizeof(env_variable));
    env_variable *current = head;
    extern char **environ;
    size_t len = strlen(name);
    for (char **ep = environ; *ep != NULL; ep++)
        if (!(strncmp(*ep, name, len) == 0 && (*ep)[len] == '='))
        {
            current->str = *ep;
            current->next = malloc(sizeof(env_variable));
        }

    char **ep = environ;
    env_variable *e = head;

    while (e != NULL)
    {
        *ep = e->str;
        ep++;
        env_variable *last = e;
        e = e->next;
        free(last);
    }
    // fill NULL to the END of environ
    if (ep != NULL)
    {
        ep++;
        *ep = NULL;
    }

    return 0;
}

static int lc = 0;
static int loop_max = 1024; // prevent endless loop.
void try_overwrite(void *p)
{
    int temp = 0;
    // check this stack address is smaller than p.
    if ((&temp) < (int *)p)
        return;

    if (lc < loop_max)
    {
        lc++;
        try_overwrite(p);
    }
}

int main(void)
{
    char *t_name = "test_name";
    char *t_val = "abc";
    printf("=====================================================\n");
    printf("Test for setenv series\n");
    printf("Test data: name=%s, value=%s\n", t_name, t_val);
    printf("=====================================================\n");
    // Original setenv
    printf("Original version setenv\n");
    setenv(t_name, t_val, 1);
    printf("Get value=%s from env with name=%s\n", getenv(t_name), t_name);

    printf("=====================================================\n");
    clearenv(); /* Erase entire environment */
    printf("Erase all, get value:%s\n", getenv(t_name));

    // Impl1: Insert string in stack to putenv()
    printf("Impl version 1 with insertion of string from stack.\n");
    setenv1(t_name, t_val, 1);
    printf("Get value=%s from env with name=%s\n", getenv(t_name), t_name);
    printf("Processing loops to overwirte stack memory.\n");
    try_overwrite((void *)getenv(t_name));
    printf("Get value=%s from env with name=%s\n", getenv(t_name), t_name);

    printf("=====================================================\n");
    clearenv(); /* Erase entire environment */
    printf("Erase all, get value:%s\n", getenv(t_name));

    // Impl2: Insert string in heap memory to putenv()
    printf("Impl version 2 with insertion of string from heap.\n");
    setenv2(t_name, t_val, 1);
    printf("Get value=%s from env with name=%s\n", getenv(t_name), t_name);
    printf("Processing loops to overwirte stack memory.\n");
    try_overwrite((void *)getenv(t_name));
    printf("Get value=%s from env with name=%s\n", getenv(t_name), t_name);

    printf("=====================================================\n");
    printf("Test for unsetenv\n");
    // Original setenv
    printf("setenv with name=%s, value=%s\n", t_name, t_val);
    setenv(t_name, t_val, 1);
    unsetenv(t_name);
    printf("Get value=%s from env with name=%s\n", getenv(t_name), t_name);
    assert(getenv(t_name) == NULL);
    return 0;
}
