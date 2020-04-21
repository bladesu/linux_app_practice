# Makefile to build library used by all programs
#
# This Makefile relies on the assumption that each C file in this
# directory belongs in the library. It takes advantage of some features
# specific to GNU make. If you don't have GNU make, remove this file
# and rename "Makefile.std" in this directory to "Makefile".
#
include ../Makefile.inc

all : ${LAP_LIB}

OBJECTS=$(patsubst %.c,%.o,$(wildcard *.c))

${LAP_LIB} : ${OBJECTS}
	${RM} ${LAP_LIB}
	${AR} rs ${LAP_LIB} *.o
	ranlib ${LAP_LIB}

error_functions.o : ename.c.inc

ename.c.inc :
	sh Build_ename.sh > ename.c.inc
	echo 1>&2 "ename.c.inc built"

clean :
	${RM} *.o ename.c.inc ${LAP_LIB}
