# Makefile to build all programs in all subdirectories

BUILD_DIRS = lib file_io process memory_allocate

all: 	
	@ echo ${BUILD_DIRS}
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}) ; \
		if test $$? -ne 0; then break; fi; done

clean: 
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
