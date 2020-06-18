include ../Makefile.common

OBJECTS = Emitter.o EmittingContext.o Interface.o ValueStack.o

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

../lib/libcmcppbe.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f *.o
	rm -f ../lib/libcmcppbe.a

