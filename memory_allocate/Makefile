include ../Makefile.inc

ALL = free_and_sbrk impl_malloc_free

all : ${ALL}

clean :  
	${RM} ${ALL} *.o

showall :
	@ echo ${ALL}

${ALL} : ${LAP_LIB}
