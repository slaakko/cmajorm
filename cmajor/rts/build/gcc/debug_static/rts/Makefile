include ../../Makefile.common

ifeq ($(PLATFORM),windows)
OBJECTS = BZ2Interface.o ZlibInterface.o Ascii.o CallStack.o Classes.o CommandLine.o Compression.o ConditionVariable.o Debug.o Directory.o Environment.o \
Error.o Fiber.o InitDone.o Io.o Math.o Memory.o Multiprecision.o Mutex.o Process.o Profile.o Random.o Screen.o Socket.o \
Statics.o String.o Thread.o Time.o UnitTest.o Unwind.o WindowsAPI.o
else
OBJECTS = BZ2Interface.o ZlibInterface.o Ascii.o CallStack.o Classes.o Compression.o ConditionVariable.o Debug.o Directory.o Environment.o \
Error.o Fiber.o InitDone.o Io.o Math.o Memory.o Multiprecision.o Mutex.o Process.o Profile.o Random.o Screen.o Socket.o \
Statics.o String.o Thread.o Time.o UnitTest.o Unwind.o 
endif

LIBRARY_NAME=libcmrtsd.a

SOURCE_PREFIX=../../../..

LIBRARY_PREFIX=../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/.. -g -O0

CFLAGS += -I$(SOURCE_PREFIX)/../.. -g -O0

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: $(SOURCE_PREFIX)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<
	
$(LIBRARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	$(RM) *.o
