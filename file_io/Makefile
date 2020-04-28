include ../Makefile.inc

ALL = copy seek_io tee cp task_create_large_file

all : ${ALL}

clean : 
	${RM} ${ALL} *.o

showall :
	@ echo ${ALL}

${ALL} : ${LAP_LIB}
