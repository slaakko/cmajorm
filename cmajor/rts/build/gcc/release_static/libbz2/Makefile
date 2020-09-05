include ../../Makefile.common

OBJECTS = blocksort.o huffman.o crctable.o randtable.o compress.o decompress.o bzlib.o

LIBRARY_NAME=libbz2.a

SOURCE_PREFIX=../../../../../system/ext/bzip2-1.0.6

LIBRARY_PREFIX=../../lib

CFLAGS += -I$(SOURCE_PREFIX) -DNDEBUG -O2

all: $(PLATFORM)

windows: $(LIBRARY_PREFIX)/$(LIBRARY_NAME)

linux:
	# nothing to do for linux
	
%.o: $(SOURCE_PREFIX)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o

