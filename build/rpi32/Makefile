CFLAGS=-g -DLINUX -w
ARCHIVE=$(HOME)/Archive
ARCHIVE_FILE=$(ARCHIVE)/cforth.tar
FLAG=BackUp.txt
CC=gcc
#
TOPDIR=../..
COMMON=$(TOPDIR)/src/common
LINUX=$(TOPDIR)/src/linux
INC=-I$(COMMON)
LIBS=-ldl
VPATH=../src

cforth:	forth.o create.o words.o consio.o Makefile 
	$(CC) $(CFLAGS) -o cforth \
    forth.o create.o words.o consio.o # $(DB) $(LIBS) 

consio.o:	$(LINUX)/consio.c $(LINUX)/consio.h 
	$(CC) -c $(CFLAGS) $(INC) $(CFLAGS) $(LINUX)/consio.c -o consio.o


forth.o:	$(COMMON)/forth.c $(COMMON)/forth.h $(COMMON)/words.h
	$(CC) -c $(CFLAGS) $(COMMON)/forth.c -o forth.o

create.o:	$(COMMON)/create.c $(COMMON)/forth.h $(COMMON)/words.h
	$(CC) -c $(CFLAGS) $(COMMON)/create.c -o create.o

words.o:	$(COMMON)/words.c $(COMMON)/words.h $(COMMON)/forth.h
	$(CC) -c $(CFLAGS) $(COMMON)/words.c -o words.o

clean:
	rm -f cforth core *.o *~ cscope.out

backup:
	rm -f $(FLAG) cforth core *.o *~ cscope.out
	( cd ../.. ; tar cvf $(ARCHIVE_FILE) ./src/cforth )
	touch BackUp.txt

install:	cforth
	cp ./cforth /usr/local/bin

pretty:
	indent -br -brs -ce forth.c
