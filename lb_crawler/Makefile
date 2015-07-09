# File: Makefile for lb_crawler
CC = gcc
CFLAGS = -Wall -pg -pedantic -std=c11
SOURCES= 
CFILES= ./src/lb_crawler.c

UTILDIR = ../util/src/
UTILFLAG = -llbutil -lcurl
UTILLIB = $(UTILDIR)liblbutil.a
UTILC = $(UTILDIR)web.c
UTILH = $(UTILDIR)web.h $(UTILDIR)common.h

# Project details
EXEC = lb_crawler
OBJS = lb_crawler.o
SRCS = $(SOURCES)

$(EXEC): $(SOURCES) $(UTILLIB)
	@$(CC) $(CFLAGS) -o $@ $(CFILES) -L../util/ -iquote../util/src/ $(UTILFLAG)

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
