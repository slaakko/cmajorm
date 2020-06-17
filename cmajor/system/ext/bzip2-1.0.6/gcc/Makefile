CC=gcc
AR=ar
RANLIB=ranlib
LDFLAGS=

CFLAGS=-Wall -Winline -O2 -g

OBJS= blocksort.o  \
	  huffman.o    \
	  crctable.o   \
	  randtable.o  \
	  compress.o   \
	  decompress.o \
	  bzlib.o

all: libbz2.a 

libbz2.a: $(OBJS)
	$(AR) cq libbz2.a $(OBJS)
	$(RANLIB) libbz2.a

clean: 
	rm -f *.o libbz2.a 

blocksort.o: ../blocksort.c
	$(CC) $(CFLAGS) -c ../blocksort.c
huffman.o: ../huffman.c
	$(CC) $(CFLAGS) -c ../huffman.c
crctable.o: ../crctable.c
	$(CC) $(CFLAGS) -c ../crctable.c
randtable.o: ../randtable.c
	$(CC) $(CFLAGS) -c ../randtable.c
compress.o: ../compress.c
	$(CC) $(CFLAGS) -c ../compress.c
decompress.o: ../decompress.c
	$(CC) $(CFLAGS) -c ../decompress.c
bzlib.o: ../bzlib.c
	$(CC) $(CFLAGS) -c ../bzlib.c
