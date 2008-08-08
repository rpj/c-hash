#CC = gcc
CFLAGS = #-DDO_TIMING -g #-DDEBUG
CC = /vendor/intel/compiler70/ia32/bin/icc
CFLAGS = -g
EXEC = db db-pipe test read-table search

all: ${EXEC}

clean:
	rm -f ${EXEC} *.o 

${EXEC}:		hash.o

hash.o:		hash.c hash.h

run-test:	test
	./test

.c.o:
	${CC} ${CFLAGS} -c $< 

# vim:set noexpandtab:
# $Id: Makefile,v 1.6 2003/08/19 23:20:17 rjoseph Exp $
