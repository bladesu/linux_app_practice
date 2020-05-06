/*
    Here I demo a really bad implementation with non-local goto with setjmp and longjmp.
    It should always avoid longjmp to a returned function. Here we can see a embarrassing
    situation that variables in the previous returned function are accessed with wrong values.
*/
#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
static jmp_buf env;

void func1()
{
    int i_obj = 1;
    char * s_obj = "a";
    switch (setjmp(env))
    {
    case 0:
        printf("===========================================\n");
        printf("First run.\n");
        printf("<int(initialized with 1):%d>\n", i_obj);
        printf("<string(initialized with 'a\\0'):%s>\n", s_obj);
        printf("===========================================\n");
        break;
    default:
        printf("===========================================\n");
        printf("Second run, return to this func1 which is returned before.\n");
        printf("<int(initialized with 1):%d>\n", i_obj);
        printf("<string(initialized with 'a\\0'):%s>\n", s_obj);
        printf("===========================================\n");
        printf("Immediately Exit\n");
        exit(EXIT_FAILURE);
        break;
    }
}

void func2()
{
    longjmp(env,1);
}


int main(void)
{
    func1();
    func2();
    exit(EXIT_SUCCESS);
}