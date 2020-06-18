include ../Makefile.common

OBJECTS = ToolChain.o ToolChains.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmtoolchain.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmtoolchain.a
