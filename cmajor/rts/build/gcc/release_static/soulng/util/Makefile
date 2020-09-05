include ../../../Makefile.common

OBJECTS = Ansi.o BinaryReader.o BinaryWriter.o CodeFormatter.o Fiber.o \
InitDone.o Json.o Log.o LogFileWriter.o MappedInputFile.o MemoryReader.o MemoryWriter.o Multiprecision.o \
Mutex.o Path.o Prime.o Process.o ProcessImpl.o Random.o \
Sha1.o Socket.o SocketFwd.o System.o TextUtils.o Time.o Unicode.o Uuid.o

LIBRARY_NAME=libcmsngutil.a

SOURCE_PREFIX=../../../../../../soulng/util

LIBRARY_PREFIX=../../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -DNDEBUG -O2

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
