include ../Makefile.inc

ALL = copy seek_io tee cp task_create_large_file task_oappend_seek atomic_append task_impl_dup task_check_duplicated_fd

all : ${ALL}

clean : 
	${RM} ${ALL} *.o

showall :
	@ echo ${ALL}

${ALL} : ${LAP_LIB}
