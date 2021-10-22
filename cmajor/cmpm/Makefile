include Makefile.common

OBJECTS = InitDone.o PortMapClient.o PortMapMessage.o PortMapServer.o Register.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmpm.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmpm.a
