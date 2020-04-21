# Common definition for all Makefile in this project.

# File hierarchy in project.
PROJECT_DIR = ..
LAP_LIB = ${PROJECT_DIR}/liblap.a
LAP_INCL_DIR = ${PROJECT_DIR}/lib/

# For compiler
CFLAGS = -Wall -g -std=c99  -I${LAP_INCL_DIR}