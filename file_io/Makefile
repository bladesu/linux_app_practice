include ../Makefile.inc

ALL = copy seek_io tee cp

all : ${ALL}

clean : 
	${RM} ${ALL} *.o

showall :
	@ echo ${ALL}

${ALL} : ${LAP_LIB}
