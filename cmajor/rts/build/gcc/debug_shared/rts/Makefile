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

LIBRARY_NAME=libcmrt371cppd.dll

SOURCE_PREFIX=../../../..

BINARY_PREFIX=../../bin

LIBRARY_PREFIX=../../lib

CXXFLAGS += -std=c++14 -I$(SOURCE_PREFIX)/../.. -I$(SOURCE_PREFIX)/.. -g -O0 -DSHARED -DRT_EXPORTS

CFLAGS += -I$(SOURCE_PREFIX)/../.. -g -O0 -DSHARED -DRT_EXPORTS

%.o: $(SOURCE_PREFIX)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: $(SOURCE_PREFIX)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<
	
$(BINARY_PREFIX)/$(LIBRARY_NAME): $(OBJECTS)
	$(CXX) -shared -o $@ $(OBJECTS) -mwindows -municode \
		-L$(LIBRARY_PREFIX) -LC:/mingw-boost/lib \
		-lcmsngcmparserd -lcmsngcmlexerd \
		-lcmsngparserd -lcmsngcmastd -lcmsngxmldomd -lcmsngxmlxmld \
		-lcmsngxmlxpathd -lcmsnglexerd -lcmehd -lcmcommond -lcmsngutild -lpdcursesd \
		-lbz2d -lzd -lwsock32 -lws2_32 -lboost_filesystem-mgw8-mt-d-x64-1_74 \
		-lboost_iostreams-mgw8-mt-d-x64-1_74 -lboost_system-mgw8-mt-d-x64-1_74 \
		-lgdiplus 

clean:
	$(RM) *.o
