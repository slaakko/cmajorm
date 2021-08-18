include ../../../Makefile.common

ifeq ($(PLATFORM),windows)
OBJECTS = Ansi.o BinaryReader.o BinaryWriter.o CodeFormatter.o Fiber.o FileLocking.o FilePtr.o \
InitDone.o Json.o Log.o LogFileWriter.o MappedInputFile.o MemoryReader.o MemoryWriter.o Multiprecision.o \
Mutex.o Path.o Prime.o Process.o ProcessImpl.o Random.o \
Sha1.o Socket.o SocketFwd.o System.o TextUtils.o Time.o Trace.o Unicode.o Uuid.o \
Stream.o BinaryStreamReader.o BinaryStreamWriter.o BufferedStream.o FileStream.o MemoryStream.o \
BZip2Stream.o Compression.o DeflateStream.o BZ2Interface.o ZLibInterface.o
else
OBJECTS = Ansi.o BinaryReader.o BinaryWriter.o CodeFormatter.o Fiber.o FileLocking.o FilePtr.o \
InitDone.o Json.o Log.o LogFileWriter.o MappedInputFile.o MemoryReader.o MemoryWriter.o Multiprecision.o \
Mutex.o Path.o Prime.o Process.o ProcessImpl.o Random.o \
Sha1.o Socket.o SocketFwd.o System.o TextUtils.o Time.o Trace.o Unicode.o Uuid.o \
Stream.o BinaryStreamReader.o BinaryStreamWriter.o BufferedStream.o FileStream.o MemoryStream.o
endif

LIBRARY_NAME=libcmsngutild.a

SOURCE_PREFIX=../../../../../../soulng/util

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -g -O0

CFLAGS += -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/../../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
