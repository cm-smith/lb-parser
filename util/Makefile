# File: Makefile
# Description: The make file is to build up the util library.
CC=gcc
CFLAGS= -Wall -pedantic -std=c11

UTILDIR=./src/
UTILFLAG=-lcurl
UTILLIB=$(UTILDIR)utils.h
UTILC=$(UTILDIR)file.c $(UTILDIR)web.c
UTILH=$(UTILDIR)file.h $(UTILDIR)common.h $(UTILDIR)web.h $(UTILDIR)utils.h

# Project details
PROGRAM = liblbutil.a
OBJS = file.o web.o
SRCS = $(UTILC) $(UTILH) $(UTILLIB)

$(PROGRAM): $(OBJS)
	@ar -cq liblbutil.a *.o

$(OBJS): $(SRCS)
	@$(CC) $(CFLAGS) -c $(SRCS)

clean:
	@rm -f *.o
	@rm -f *.a
	@rm -f ./src/*.gch
