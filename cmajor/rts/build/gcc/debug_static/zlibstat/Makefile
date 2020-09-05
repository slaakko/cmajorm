include ../../Makefile.common

OBJECTS = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o inffast.o inflate.o infback.o inftrees.o trees.o uncompr.o zutil.o

LIBRARY_NAME=libzd.a

SOURCE_PREFIX=../../../../../system/ext/zlib-1.2.11

LIBRARY_PREFIX=../../lib

all: $(PLATFORM)

windows: $(LIBRARY_PREFIX)/$(LIBRARY_NAME)

linux:
	# nothing to do for linux

CFLAGS += -I$(SOURCE_PREFIX) -g -O0

%.o: $(SOURCE_PREFIX)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
