# File: Makefile for lb_indexer
CC = gcc
CFLAGS = -Wall -pg -pedantic -std=c11
SOURCES= ./src/distrib.h 
CFILES= ./src/distrib.c ./src/lb_indexer.c

UTILDIR = ../util/src/
UTILFLAG = -llbutil -lcurl
UTILLIB=$(UTILDIR)liblbutil.a
UTILC = $(UTILDIR)web.c $(UTILDIR)file.c
UTILH = $(UTILDIR)web.h $(UTILDIR)common.h $(UTILDIR)file.h

# Project details
EXEC = lb_indexer
OBJS = lb_indexer.o distrib.o
SRCS=$(SOURCES)

$(EXEC): $(SOURCES) $(UTILLIB)
	@$(CC) $(CFLAGS) -o $@ $(CFILES) -L../util/ -iquote../util/src/ $(UTILFLAG)
	@echo "Build successful"

$(OBJS): $(SRCS)
	@$(CC) $(CFLAGS) -c $(CFILES)

$(UTILLIB): $(UTILC) $(UTILH)
	@cd ../util/ && $(MAKE) clean && $(MAKE);

debug: $(SRCS)
	@$(CC) $(CFLAGS) -g -ggdb3 -c $(SRCS)
	@$(CC) $(CFLAGS) -g -ggdb3 -o $(EXEC) $(OBJS) $(UTILFLAG)

clean:
	@rm -f *.o
	@rm -f $(EXEC)
	@rm -f ./src/*.gch
	@cd ../util && $(MAKE) clean
	@echo "Clean successful"
