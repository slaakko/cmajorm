include ../Makefile.common

OBJECTS = Ansi.o BinaryReader.o BinaryWriter.o CodeFormatter.o Fiber.o FileLocking.o FilePtr.o InitDone.o Json.o Log.o LogFileWriter.o \
MappedInputFile.o MemoryReader.o MemoryWriter.o Multiprecision.o Mutex.o Path.o Prime.o \
Process.o ProcessImpl.o Random.o Sha1.o Socket.o SocketFwd.o System.o TextUtils.o Time.o Trace.o Unicode.o Uuid.o \
BinaryStreamWriter.o BinaryStreamReader.o Stream.o BufferedStream.o FileStream.o FileUtil.o MemoryStream.o 

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	
../../lib/libcmsngutil.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../../lib/libcmsngutil.a
