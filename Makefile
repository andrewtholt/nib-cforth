# DB=../../smallDatabase/db.o
CFLAGS=-g -m32 -DLINUX -DFILESYSTEM # -DDB
ARCHIVE=$(HOME)/Archive
ARCHIVE_FILE=$(ARCHIVE)/cforth.tar
FLAG=BackUp.txt
CC=gcc
LIBS=-ldl

cforth:	forth.o create.o words.o Makefile # access_db.o
	$(CC) $(CFLAGS) -o cforth \
    forth.o create.o words.o $(DB) $(LIBS) #  access_db.o

# cforth:	forth.o create.o words.o access_db.o Makefile $(DB)
#	$(CC) $(CFLAGS) forth.o create.o words.o $(DB) \
#	access_db.o $(LIBS) \
#	-o cforth

forth.o:	forth.c forth.h words.h
	$(CC) -c $(CFLAGS) forth.c -o forth.o

create.o:	create.c forth.h words.h
	$(CC) -c $(CFLAGS) create.c -o create.o

words.o:	words.c words.h forth.h
	$(CC) -c $(CFLAGS) words.c -o words.o

# access_db.o:	access_db.c db.h
#	$(CC) -c $(CFLAGS) access_db.c -o access_db.o

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
