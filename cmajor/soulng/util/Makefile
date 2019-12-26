include ../Makefile.common

OBJECTS = BinaryReader.o BinaryWriter.o CodeFormatter.o InitDone.o Json.o Log.o MappedInputFile.o MemoryReader.o MemoryWriter.o Mutex.o Path.o Prime.o Random.o \
Sha1.o System.o TextUtils.o Time.o Unicode.o Uuid.o

%o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngutil.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngutil.a
