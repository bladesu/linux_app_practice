/*
    Here I demo a really bad implementation with non-local goto with setjmp and longjmp.
    It should always avoid longjmp to a returned function. Here we can see a embarrassing
    situation 
    case 1) that variables in the previous returned function are accessed with wrong values.
    case 2) segmentation fault when return to setjmp.
*/
#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
static jmp_buf env1;
static jmp_buf env2;

void setJumpChild(jmp_buf env)
{
    setjmp(env);
}
void setJumpMain(jmp_buf env){
   setJumpChild(env); 
}
void func1(jmp_buf env)
{
    int i_obj = 1;
    char *s_obj = "a";
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
        break;
    }
}

void func2(jmp_buf env)
{
    longjmp(env, 1);
}

static int r = 0;
void recursive()
{
    r++;
    if (r > 10)
        return;
    printf("do recursive loop~\n");
    recursive();
}
int main(void)
{
    // case 1: unexpected data value
    func1(env1);
    func2(env1);
    // case 2: Segmentation fault
    setJumpMain(env2);
    recursive();
    func2(env2);
    exit(EXIT_SUCCESS);
}