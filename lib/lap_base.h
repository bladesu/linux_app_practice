#ifndef LAP_BASE_H
#define LAP_BASE_H

#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
#include "error_functions.h"  /* Declares our error-handling functions */
//#include "get_num.h" 
/* Declares our functions for handling numeric arguments (getInt(), getLong()) */
                

typedef enum { FALSE, TRUE } Boolean;

#endif 