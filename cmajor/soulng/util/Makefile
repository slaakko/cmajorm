include ../Makefile.common

OBJECTS = BinaryReader.o BinaryWriter.o CodeFormatter.o Fiber.o InitDone.o Json.o Log.o MappedInputFile.o MemoryReader.o MemoryWriter.o Multiprecision.o Mutex.o Path.o Prime.o \
Process.o ProcessImpl.o Random.o Sha1.o Socket.o System.o TextUtils.o Time.o Unicode.o Uuid.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngutil.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngutil.a
