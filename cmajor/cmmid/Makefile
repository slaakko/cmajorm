include ../Makefile.common

OBJECTS = BackEnd.o CmcppBackEnd.o InitDone.o Interface.o LlvmBackEnd.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmmid.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmmid.a
