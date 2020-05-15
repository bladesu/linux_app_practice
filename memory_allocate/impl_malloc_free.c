#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
typedef struct __MBL
{
    size_t length;
    struct __MBL *last;
    struct __MBL *next;
} MBLOCK;
// __MIN_LENG should be larger than certain value
// which make every segment of returning heap memory larger than smallest MBLOCK.
#define __MIN_LENG (sizeof(void *) + sizeof(void *))
#define __ALLOCK_INTERVAL (64 * __MIN_LENG)
static const size_t _MIN_LENG = (size_t)__MIN_LENG;
static const size_t _ALLOCK_INTERVAL = (size_t)__ALLOCK_INTERVAL;
static void *__CURRENT_HEAP_PTR;
static MBLOCK *__HEAD; // head of free list

void show_current_free_list(void)
{
    if (__HEAD == NULL)
    {
        printf("No free list to show\n");
        return;
    }
    printf("> begin ------------------------------\n");
    printf("head:%p\n", __HEAD);
    printf("program break:%p\n", __CURRENT_HEAP_PTR);
    MBLOCK *block = (MBLOCK *)__HEAD;
    int s = 1;
    do
    {
        printf("segment %d  address:%14p, length:%14ld, last:%14p, next:%14p\n", s, block, block->length, block->last, block->next);
        block = block->next;
        s++;
    } while (block != NULL);
    printf("> end   ------------------------------\n");
}

void *__first_get_expanded_heap(intptr_t __delta)
{
    void *ptr = sbrk(__delta);
    if (ptr == (void *)-1)
    {
        // FAIL TO MOVE program break
        errno = ENOMEM;
        return NULL;
    }
    else
        return ptr;
}
/*
    RETURN new heap pointer(program break) get from brk, or return NULL if error.
*/
void *__get_expanded_heap(intptr_t __delta)
{
    void *ptr = __first_get_expanded_heap(__delta);
    if (ptr == (void *)-1)
        return NULL;
    else if (ptr == __CURRENT_HEAP_PTR)
        return ptr + __delta;
    return NULL; // unpredicable result.
}

/*
    split block into two pieces.
 */
void split(MBLOCK *_block, const size_t _length)
{
    // 1. Original: split one free block into two.
    //
    //                       [<------------------------ "block->length" ---------------------------->]
    // Current Heap:[lengthL][addressL][addressL][<-               __free_space1                   ->]
    //              ^ _block                                                                         .
    // 2.                                                                                            .
    // Current Heap:[lengthL][<-   allocated space    ->][lengthL][addressL][addressL][ __free_space2]
    //              .        [<------- "_length" ------>]         [<---------- NEW LENGTH----------->]
    //              .                                   .                                            .
    //              [<------------- left -------------->][<-----------------right------------------->]
    MBLOCK *right = (MBLOCK *)(((void *)_block) + _length + sizeof(size_t));
    right->next = _block->next;
    right->last = _block->last;
    right->length = _block->length - _length - sizeof(size_t);

    MBLOCK *left = _block;
    left->length = _length;
    // It is __HEAD
    if (_block->last == NULL)
    {
        // move __HEAD to right
        __HEAD = right;
    }
}
/*
Initialize heap space controled by this malloc implementation.
Return current heap pointer, return NULL if error.
*/
void *__init_my_malloc()
{
    void *ptr;
    // 1. Initialize __CURRENT_HEAP_PTR, it need assigned __CURRENT_HEAP_PTR in this first time brk invocation.
    if ((ptr = __first_get_expanded_heap(0)) != NULL)
        __CURRENT_HEAP_PTR = ptr;
    else
        return NULL;
    // 2. initialize __HEAD.
    if ((ptr = __get_expanded_heap(sizeof(MBLOCK))) != NULL)
    {
        __CURRENT_HEAP_PTR = ptr;
        __HEAD = (MBLOCK *)__CURRENT_HEAP_PTR - 1;
        __HEAD->length = sizeof(MBLOCK) - sizeof(size_t);
        __HEAD->last = NULL;
        __HEAD->next = NULL;
    }
    else
        return NULL;
    return __CURRENT_HEAP_PTR;
}
/*
    append new allocated heap space to last_block.
*/
void *__expand_newblock(MBLOCK *last_block, intptr_t _length)
{
    /* 1. Original heap */
    /* 2. After expansion with requirement of "_length" heap space */

    // 1.
    // Current Heap:[lengthL][addressL][addressL]
    //                                          ^- heap pointer(program break)
    // 2.
    // Current Heap:[lengthL][addressL][addressL][   "_length" - 2 * addressL    ]
    //                       [<----------------- "_length" --------------------->]
    // Expanded  (mem_size)                      [<----------------------------->][lengthL][addressL][addressL]
    //                                                                            [<- new free-list element ->]
    //                                                                                                        ^- new heap pointer(program break)
    // Therefore, Expanded = _length + (- 2 * addressL + 2 * addressL)  + lengthL. = _length + lengthL
    // sizeof for a allocated memory segment: "_length + sizeof(size_t)"

    intptr_t mem_size = (_length > _ALLOCK_INTERVAL) ? _length + sizeof(size_t) : _ALLOCK_INTERVAL;
    void *ptr;
    // EXPAND __CURRENT_HEAP_PTR
    if ((ptr = __get_expanded_heap(mem_size)) != NULL)
        __CURRENT_HEAP_PTR = ptr;
    else
        return NULL;

    // attach new block
    last_block->length = _length;
    last_block->next = __CURRENT_HEAP_PTR - sizeof(MBLOCK) + sizeof(size_t);
    // fill data of the new block.
    last_block->next->last = last_block;
    last_block = last_block->next;
    last_block->length = sizeof(MBLOCK) - sizeof(size_t);
    last_block->next = NULL;
    return ptr;
}

int __rule_length_eq(size_t __block_legnth, size_t __length)
{
    return __block_legnth == __length;
}
int __rule_length_avali(size_t __block_legnth, size_t __length)
{
    return __block_legnth >= (__length + sizeof(size_t));
}
MBLOCK *execute_rule(MBLOCK *block, size_t _length, int (*rule)(size_t, size_t))
{
    /* 
        Original block has free space with "block->length" bytes,
        and then cut meomry to the process as allocated heap memory with "_length" bytes required.
    */
    /* 1. Original heap */
    /* 2. After spliting _length space */

    // 1.
    //                       [<------------------------ "block->length" ----------------------------->]
    // Current Heap:[lengthL][addressL][addressL][<--                __free_space1                 -->]
    //              .                                                                                 .
    // 2.           .                                                                                 .
    // Current Heap:[lengthL][    allocated space       ][lengthL][addressL][addressL][ __free_space2 ]
    //                       [<------- "_length" ------>]
    //
    // "block->length" = (_length + lengthL) + __free_space2
    // "block->length" >= _length + lengthL
    do
    {
        if (rule(block->length, _length))
        {
            split(block, _length);
            return (MBLOCK *)((void *)block + sizeof(size_t));
        }
        else
            block = block->next;

    } while (block->next != NULL);
    // do not apply to least one in the free list
    return NULL;
}
/* by man malloc
The malloc() and calloc() functions return a pointer to the allocated
       memory, which is suitably aligned for any built-in type.  On error,
       these functions return NULL.  NULL may also be returned by a
       successful call to malloc() with a size of zero,
*/
void *my_malloc(size_t __size)
{
    // initialization
    if (__CURRENT_HEAP_PTR == NULL)
    {
        // Initialize __HEAD and __CURRENT_HEAP_PTR.
        if (__init_my_malloc() == NULL)
            return NULL;
        // Expand __CURRENT_HEAP_PTR and attach to __HEAD.
        if (__expand_newblock(__HEAD, _ALLOCK_INTERVAL) == NULL)
            return NULL;
    }

    if (__size == 0)
        return NULL;

    // Modify required length to multiple of MIN_SIZE.
    size_t remainder = __size % _MIN_LENG;
    __size = __size + ((remainder > 0) ? (_MIN_LENG - remainder) : 0);
    printf("Requiring %lu bytes of heap space.\n", __size);

    MBLOCK *block;
    // My defined Rule 1: return the first size totally-fitted.
    if ((block = execute_rule(__HEAD, __size, __rule_length_eq)) != NULL)
        return (void *)block;
    // My defined Rule 2: return the truncated block.
    if ((block = execute_rule(__HEAD, __size, __rule_length_avali)) != NULL)
        return (void *)block;

    // My defined Rule 3: no avaliable free block to release, expand heap space
    if (__expand_newblock((__CURRENT_HEAP_PTR - sizeof(MBLOCK) + sizeof(size_t)), __size) == NULL)
        return NULL;

    // My defined Rule 1: return the first size totally-fitted.
    if ((block = execute_rule(__HEAD, __size, __rule_length_eq)) != NULL)
        return (void *)block;

    // My defined Rule 2: return the truncated block.
    if ((block = execute_rule(__HEAD, __size, __rule_length_avali)) != NULL)
        return (void *)block;

    return NULL;
}

void my_free(void *__ptr)
{
    // case 1
    // [ block to join ]|       [lengthL][LAST][NEXT][ data  ]
    // ^_block          ^end    ^add1                        ^ add2 = add1 + sizeof(size_t) + lengthL
    // case 2
    // [ block to join ][ first block in free list ]
    // ^_block          ^end
    // case 3
    // [ free block i ]  [ block to join ]|  [ free block i+1 ]
    //                   ^_block          ^end
    MBLOCK *_block = (MBLOCK *)((size_t *)__ptr - 1);
    void *end = (char *)__ptr + _block->length;
    MBLOCK *_current = __HEAD;

    // case 1
    if (end < (void *)_current)
    {
        __HEAD = _block;
        __HEAD->last = NULL;
        __HEAD->next = _current;
        __HEAD->next->last = __HEAD;
        return;
    }
    // case 2
    if (end == (void *)_current)
    {
        __HEAD = _block;
        __HEAD->length = (size_t)((char *)_current + *(size_t *)_current - (char *)_block);
        __HEAD->last = NULL;
        __HEAD->next = _current->next;
        __HEAD->next->last = __HEAD;
        return;
    }
    // case 3
    do
    {
        // check _block is in the middle.
        if (_current < _block && _block < _current->next)
        {
            void *_current_end = (char *)_current + sizeof(size_t) + _current->length;
            // check whether it is adjacent.
            if (_current_end == _block)
            {
                _current->length = _current->length + sizeof(size_t) + _block->length;
                // merge following block but not for last one.
                if (end == _current->next && _current->next->next != NULL)
                {
                    _current->length = _current->length + sizeof(size_t) + _current->next->length;
                    _current->next = _current->next->next;
                }
                return;
            }
            else
            {
                _block->last = _current;
                // merge following block but not for last one. 
                if (end == _current->next && _current->next->next != NULL)
                {
                    _block->length = _block->length + sizeof(size_t) + _current->next->length;
                    _block->next = _current->next->next;
                }
                else
                {
                    _block->next = _current->next;
                    _current->next->last = _block;
                }
                _current->next = _block;
                return;
            }
        }
        _current = _current->next;
    } while (_block != NULL);
}

int main(void)
{
    printf("sizeof(MBLOCK):%ld\n", sizeof(MBLOCK));
    printf("sizeof(size_t):%ld\n", sizeof(size_t));
    printf("sizeof(void *):%ld\n", sizeof(void *));
    printf("======================================================================\n");
    printf("Initialization of heap space.\n");
    printf("First free block is expected to have free length =%ld\n", _ALLOCK_INTERVAL);
    printf("Last free block is expected to have free length =%ld\n", 2 * sizeof(void *));
    //According spec, input = zero malloc should return NUL
    assert(my_malloc(0) == NULL);
    show_current_free_list();
    printf("======================================================================\n");
    // test locate 1 integer with heap space,
    printf("Locate heap space with available length >= %ld bytes\n", sizeof(int));
    int *ptr1 = my_malloc(sizeof(int));
    *ptr1 = 1;

    printf("Examination1: Stored value, at heap address=%p, int value= %d\n", ptr1, *ptr1);
    show_current_free_list();
    printf("Locate other heap space with available length >= %ld bytes\n", sizeof(int));
    int *ptr2 = my_malloc(sizeof(int));
    show_current_free_list();

    //test my_free
    my_free(ptr1);
    show_current_free_list();
    my_free(ptr2);
    show_current_free_list();
    return 0;
}