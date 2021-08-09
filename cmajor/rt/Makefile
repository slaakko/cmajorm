include ../Makefile.common

OBJECTS = Ascii.o CallStack.o Classes.o Compression.o ConditionVariable.o Debug.o Directory.o Environment.o Error.o Fiber.o InitDone.o Io.o Math.o Memory.o \
Multiprecision.o Mutex.o Process.o Profile.o Random.o Screen.o Socket.o Statics.o String.o Thread.o Time.o Trace.o UnitTest.o Unwind.o BZ2Interface.o ZlibInterface.o

LIBRARIES = ../lib/libcmsngutil.a ../lib/libcmsnglexer.a ../lib/libcmsngparser.a ../lib/libcmsngxmlxml.a ../lib/libcmsngxmldom.a ../lib/libcmsngxmlxpath.a \
../lib/libcmsngcmast.a ../lib/libcmsngcmcmlexer.a ../lib/libcmsngcmcmparser.a ../lib/libcmcommon.a ../lib/libcmir.a ../lib/libcmllvm.a ../lib/libcmsymbols.a \
../lib/libcmeh.a

CXXFLAGS += -L../lib

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ifeq ($(config),debug)
soname=libcmrtd.so.4.1.0
sobasename=libcmrtd
else
soname=libcmrt.so.4.1.0
sobasename=libcmrt
endif

../lib/$(soname): $(OBJECTS) $(LIBRARIES)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJECTS) -lcmeh -lcmsymbols -lcmllvm -lcmir -lcmcommon -lcmsngcmcmparser -lcmsngcmcmlexer -lcmsngcmast \
-lcmsngxmlxpath -lcmsngxmldom -lcmsngxmlxml -lcmsngparser -lcmsnglexer -lcmsngutil -ldl -ltinfo -lgnutls -lncurses
	ln -f ../lib/$(soname) ../lib/$(sobasename).so.4
	ln -f ../lib/$(soname) ../lib/$(sobasename).so

clean:
	rm -f *.o
	rm -f ../lib/$(soname)
